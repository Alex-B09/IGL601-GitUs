#ifndef GIT_INIT__H
#define GIT_INIT__H

enum class InitErrorCode
{
	InvalidDirectory,
	FileSystemError,
	NoError
};

InitErrorCode init();

#endif