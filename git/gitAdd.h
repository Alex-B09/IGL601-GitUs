#ifndef GIT_ADD__H
#define GIT_ADD__H

#include <string>

enum class AddErrorCode
{
	InvalidFile,
	InvalidGitFolder,
	InvalidGitIndex,
	InvalidIO,
	NoError
};

AddErrorCode Add(std::string filePath);

void test1();

#endif
