#include "gitCommit.h"

#include "gitUtils.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

struct ElementContent
{
	std::vector<unsigned int> sha;
	std::string name;
	bool isDirectory = false;
};

std::vector<unsigned int> GenerateTrees(boost::filesystem::path currentPath)
{
	const std::string DIRETORY_CODE = "040000";
	const std::string BLOB_CODE = "100644";
	// on prend les fichiers présents et on ajoute dans une 
	
	boost::system::error_code code;
	bool isDirectory = boost::filesystem::is_directory(currentPath, code);
	if (code.failed())
	{
		return {};
	}
	else if (isDirectory)
	{
		std::vector<ElementContent> content;
		auto fileContent = boost::filesystem::directory_iterator(currentPath);
		for (; fileContent != boost::filesystem::directory_iterator(); ++fileContent)
		{
			auto path = fileContent->path();
			auto insideElement = GenerateTrees(path);
			auto filename = path.filename().string();

			if (insideElement.size() > 0)
			{
				ElementContent elementEntry;
				elementEntry.sha = insideElement;
				elementEntry.name = filename;

				boost::system::error_code code;
				elementEntry.isDirectory = boost::filesystem::is_directory(path, code);

				content.push_back(elementEntry);
			}
		}
		
		std::string treeContent;
		for (auto elem : content)
		{
			if (elem.isDirectory)
			{
				treeContent += DIRETORY_CODE;
			}
			else
			{
				treeContent += BLOB_CODE;
			}
			treeContent += " ";

			//add sha
			std::stringstream fileStream;
			for (auto partialSha : elem.sha)
			{
				fileStream << std::hex << partialSha;
			}

			treeContent += fileStream.str();

			//add tab
			treeContent += "\t";

			//add filename
			treeContent += elem.name;

			//add linefeed
			treeContent += "\n"; // important to be \n and not any other thing
		}
		// blob the thing
		std::vector<char> vectorizedTreeContent(treeContent.begin(), treeContent.end());
		return GitUtils::GetHash(vectorizedTreeContent);
	}
	else // a file
	{
		std::ifstream file(currentPath.c_str());
		if (file.is_open())
		{
			std::string fileContent{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
			std::vector<char> vectorizedTreeContent(fileContent.begin(), fileContent.end());
			return GitUtils::GetHash(vectorizedTreeContent);
		}
	}

	return {};
}


CommitErrorCode Commit(std::string )
{
	// check if is git folder
	if (!GitUtils::isValidGitFolder())
	{
		return CommitErrorCode::InvalidGitFolder;
	}

	// 

	return CommitErrorCode::InvalidIO;
}
