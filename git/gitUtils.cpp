#include "gitUtils.h"

#include <fstream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>


using std::vector;
using std::string;

vector<string> GitUtils::GetSeparatedArgs(string cmd)
{
	vector<string> separatedString;

	string partialString;
	bool isGrouped = false;
	for (char c : cmd)
	{
		if (c == '"')
		{
			if (isGrouped)
			{
				separatedString.push_back(partialString);
				partialString.clear();
			}
			isGrouped = !isGrouped;
		}
		else if (c == ' ' && !isGrouped)
		{
			if (!partialString.empty())
			{
				separatedString.push_back(partialString);
				partialString.clear();
			}
		}
		else
		{
			partialString += c;
		}
	}

	if (!partialString.empty())
	{
		separatedString.push_back(partialString);
		partialString.clear();
	}

	return separatedString;
}

GitUtils::cmdCategory GitUtils::GetCmdCategory(string rawCategory)
{
	if (rawCategory == CMD_INIT)
	{
		return cmdCategory::init;
	}
	else if (rawCategory == CMD_ADD)
	{
		return cmdCategory::add;
	}
	else if (rawCategory == CMD_COMMIT)
	{
		return cmdCategory::commit;
	}

	return cmdCategory::error;
}

int GitUtils::GetHelpArg(vector<string> args)
{
	auto position = std::find(args.cbegin(), args.cend(), CMD_ARG_HELP);

	if (position != args.cend())
	{
		return std::distance(args.cbegin(), position);
	}
	return -1;
}

bool GitUtils::isValidGitFolder()
{
	auto path = boost::filesystem::current_path();
	const auto pathFolder = path.append(".git");	

	boost::system::error_code code;
	bool gitExists = boost::filesystem::exists(pathFolder, code);
	return gitExists;
}

GitUtils::indexInfo GitUtils::ReadIndexFile()
{
	indexInfo indexContent;

	auto path = boost::filesystem::current_path();
	const auto indexFilePath = path.append(".git").append("index");

	boost::system::error_code code;
	bool exists = boost::filesystem::exists(indexFilePath, code);
	if (!exists || code.failed())
	{
		return indexContent;
	}

	std::stringstream fileStream;
	std::fstream file(indexFilePath.c_str());

	if (!file.is_open() || file.rdbuf()->in_avail() == 0)
	{
		return indexContent;
	}

	fileStream << file.rdbuf();

	int nb;
	fileStream >> nb;

	//TODO not good...
	//the sha is in one block
	for (int i = 0; i < nb; ++i)
	{
		std::string shaString;
		std::string filename;

		fileStream >> shaString >> filename;

		IndexEntry entry;
		for (int i = 0; i < 5; ++i)
		{
			std::string shaPartString = shaString.substr(i*8, 8);
			unsigned int shaPart = std::stoul(shaPartString, nullptr, 16);
			entry.sha.push_back(shaPart);
		}

		entry.fileName = filename;
		
		indexContent.push_back(entry);
	}

	return indexContent;
}

bool GitUtils::WriteToIndexFile(GitUtils::indexInfo files)
{
	auto objectPath = boost::filesystem::current_path().append(".git").append(GitUtils::INDEX_FILE);

	std::stringstream fileStream;
	fileStream << files.size() << std::endl;

	for (auto fileEntry : files)
	{
		for (auto partialId : fileEntry.sha)
		{
			fileStream << std::hex << partialId;
		}
		fileStream << " " << fileEntry.fileName << std::endl;
	}

	std::ofstream outputFile(objectPath.c_str());
	outputFile << fileStream.rdbuf();
	outputFile.close();

	return true;
}

std::vector<unsigned int> GitUtils::GetHash(std::vector<char> content)
{
	using boost::uuids::detail::sha1;
	sha1 sha;
	sha.process_bytes(content.data(), content.size());

	// bof... would prefer array...
	unsigned int hash[5];
	sha.get_digest(hash);

	std::vector<unsigned int> hashVector;

	for (auto elem : hash)
	{
		hashVector.push_back(elem);
	}

	return hashVector;
}
