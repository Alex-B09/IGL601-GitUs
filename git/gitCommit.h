#ifndef GIT_COMMIT__H
#define GIT_COMMIT__H

#include <string>

enum class CommitErrorCode
{
	InvalidFile,
	InvalidGitFolder,
	InvalidGitIndex,
	InvalidIO,
	NoError
};

CommitErrorCode Commit(std::string filePath);

#endif
