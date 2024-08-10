#include "gitAdd.h"

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


bool isValidFile(boost::filesystem::path filePath)
{
	boost::system::error_code code;
	bool gitExists = boost::filesystem::exists(filePath, code);
	return gitExists && !code.failed();
}

std::vector<char> GetBlobableContent(std::ifstream & file)
{
	std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	int contentLenght = content.length();

	std::string infoType = "blob";
	infoType += " " + std::to_string(content.length());

	std::vector<char> nonDestructiveString; // string trunks at the \0 -- a vector does not
	nonDestructiveString.assign(infoType.begin(), infoType.end());
	nonDestructiveString.push_back('\0'); // this is the problematic line for a string
	nonDestructiveString.insert(nonDestructiveString.end(), content.begin(), content.end());

	return nonDestructiveString;
}

auto GetDeflatedContent(std::vector<char> content)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input>in;
	in.push(boost::iostreams::zlib_compressor());
	in.push(boost::make_iterator_range(content));
	std::string deflated;
	boost::iostreams::copy(in, boost::iostreams::back_inserter(deflated));

	return deflated;
}

bool WriteHashedFile(std::vector<unsigned int> hash, std::string deflatedContent)
{
	auto objectPath = boost::filesystem::current_path().append(".git/objects");

	std::stringstream fileNameStream;
	for (auto elem: hash) 
	{
		fileNameStream << std::hex << elem;
	}

	std::string fileName = fileNameStream.str();
	// get folder 2 characters
	std::string directoryName = fileName.substr(0, 2);
	objectPath.append(directoryName);

	boost::system::error_code code;
	boost::filesystem::create_directory(objectPath, code);
	if (code.failed())
	{
		return false;
	}
	
	auto outputFilePath = objectPath;
	auto reducedFileName = fileName.substr(2);
	outputFilePath.append(reducedFileName);

	std::ofstream outputFile(reducedFileName.c_str());
	outputFile << deflatedContent;
	outputFile.close();

	return true;
}

bool AddFileToIndex(std::string fileName, std::vector<unsigned int> hash)
{
	GitUtils::IndexEntry entry;
	entry.sha = hash;
	entry.fileName = fileName;
	
	auto indexContent = GitUtils::ReadIndexFile();
	indexContent.push_back(entry);

	return GitUtils::WriteToIndexFile(indexContent);
}

AddErrorCode Add(std::string filePath)
{
	// check if is git folder
	if (!GitUtils::isValidGitFolder())
	{
		return AddErrorCode::InvalidGitFolder;
	}

	// valider le fichier -- il existe
	const auto path = boost::filesystem::current_path().append(filePath);
	if (!isValidFile(path))
	{
		return AddErrorCode::InvalidFile;
	}
	// open file
	std::ifstream addedFile(path.c_str());
	// should not happen...but
	if (!addedFile.is_open())
	{
		return AddErrorCode::InvalidFile;
	}

	// voir si il y a un index
	const auto pathIndex = boost::filesystem::current_path().append(".git/index");
	if (!isValidFile(pathIndex))
	{
		return AddErrorCode::InvalidGitIndex;
	}
	// open index
	std::ifstream indexFile(path.c_str());
	// should not happen...but
	if (!indexFile.is_open())
	{
		return AddErrorCode::InvalidGitIndex;
	}

	// blober - contenu
	auto blobableContent = GetBlobableContent(addedFile);

	// hash du fichier
	auto hash = GitUtils::GetHash(blobableContent);
	auto deflatedContent = GetDeflatedContent(blobableContent);

	// write to disk
	if (!WriteHashedFile(hash, deflatedContent))
	{
		return AddErrorCode::InvalidIO;
	}

	// add to git index
	AddFileToIndex(filePath, hash);

	return AddErrorCode::NoError;
}
