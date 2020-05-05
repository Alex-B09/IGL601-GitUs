#include "catch.hpp"

#include <gitInit.h>

#include <string>
#include <boost/filesystem.hpp>

TEST_CASE("InitTestCase - Invalid")
{
	auto path = boost::filesystem::current_path();
	auto testPath = path;
	testPath.append("testInit");

	boost::filesystem::create_directory(testPath);
	boost::filesystem::current_path(testPath);
	
	SECTION("Already existing .git")
	{
		auto gitPath = testPath;
		boost::filesystem::create_directory(gitPath.append(".git"));


		REQUIRE(init() == InitErrorCode::InvalidDirectory);
	}

	SECTION("Non existant .git")
	{
		REQUIRE(init() == InitErrorCode::NoError);

		auto gitPath = testPath;
		boost::filesystem::create_directory(gitPath.append(".git"));
		// validate index file and objects folder
		auto indexPath = gitPath;
		auto objectPath = gitPath;

		indexPath.append("index");
		objectPath.append("objects");

		boost::system::error_code codeObject;
		boost::system::error_code codeObjectEmpty;
		bool isObjectDirectory = boost::filesystem::is_directory(objectPath, codeObject);
		bool isObjectEmpty = boost::filesystem::is_empty(objectPath, codeObjectEmpty);

		REQUIRE((isObjectDirectory 
				 && isObjectEmpty 
				 && !codeObject.failed() 
				 && !codeObjectEmpty.failed()));

		boost::system::error_code codeIndex;
		boost::system::error_code codeIndexEmpty;
		bool isIndexFile = boost::filesystem::is_regular_file(indexPath, codeIndex);
		bool isIndexEmpty = boost::filesystem::is_empty(objectPath, codeObjectEmpty);

		REQUIRE((isIndexFile 
				 && isIndexEmpty 
				 && !codeIndex.failed() 
				 && !codeIndexEmpty.failed()));
	}

	boost::filesystem::current_path(path); // return to original
	boost::filesystem::remove_all(testPath);
}
