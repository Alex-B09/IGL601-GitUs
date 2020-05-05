#include "gitInit.h"

#include "gitUtils.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

InitErrorCode init()
{
	namespace fs = boost::filesystem;

	if (!GitUtils::isValidGitFolder())
	{
		auto currentPath = fs::current_path();

		// 1 - Create .git
		currentPath.append(GitUtils::GIT_DIRECTORY);
		
		boost::system::error_code code;

		bool isDirectoryCreated = fs::create_directory(currentPath, code);

		if (!isDirectoryCreated || code.failed())
		{
			return InitErrorCode::FileSystemError;
		}

		// 2 - create index file-- empty
		auto indexFilePath = currentPath;
		indexFilePath.append(GitUtils::INDEX_FILE);
		std::ofstream file(indexFilePath.c_str());
		file.close();

		// 3 - create objects directory -- empty
		currentPath.append(GitUtils::OBJECT_DIRECTORY);
		isDirectoryCreated = fs::create_directory(currentPath, code);

		if (!isDirectoryCreated || code.failed())
		{
			return InitErrorCode::FileSystemError;
		}

		return InitErrorCode::NoError;
	}
	return InitErrorCode::InvalidDirectory;
}
