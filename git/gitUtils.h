#ifndef GIT_UTILS__H
#define GIT_UTILS__H

// gitutils.h
// stuff needed for all commands 
// or things that are outside of the gitus scope

#include <vector>
#include <string>

namespace GitUtils
{
	using std::vector;
	using std::string;

	enum class cmdCategory
	{
		add,
		init,
		commit,
		error
	};

	const string CMD_ADD = "add";
	const string CMD_COMMIT = "commit";
	const string CMD_INIT = "init";
	const string CMD_ARG_HELP = "--help";

	const auto GIT_DIRECTORY = ".git";
	const auto INDEX_FILE = "index";
	const auto OBJECT_DIRECTORY = "objects";

	vector<string> GetSeparatedArgs(string cmd);
	cmdCategory GetCmdCategory(string rawCategory);

	// returns -1 if not there, index position otherwise
	int GetHelpArg(vector<string> args);

	bool isValidGitFolder();

	struct IndexEntry
	{
		std::vector<unsigned int> sha;
		std::string fileName;

	};

	using indexInfo = std::vector<IndexEntry>;

	indexInfo ReadIndexFile();
	bool WriteToIndexFile(GitUtils::indexInfo files);

	std::vector<unsigned int> GetHash(std::vector<char> content);
}


#endif