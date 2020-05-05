#include "catch.hpp"

#include <gitAdd.h>
#include <gitUtils.h>

#include <string>
#include <boost/filesystem.hpp>

TEST_CASE("AddTestCase")
{
	std::string fileContent = "blahhhhhhh\n";
	std::string fileName = "test1212.txt";

	std::string targetIndexContent = "1\n114c710f11d2dd6741a2d3b51362e1ab9de24428 test1212.txt\n";

	auto path = boost::filesystem::current_path();

	auto testPath = path;
	testPath.append("testAdd-Generated");
	boost::filesystem::create_directory(testPath);

	boost::filesystem::current_path(testPath);

	std::ofstream contentFile(fileName);
	contentFile << fileContent;
	contentFile.close();

	auto gitPath = testPath;
	gitPath.append(".git");
	boost::filesystem::create_directory(gitPath);
	boost::filesystem::current_path(gitPath);
	
	auto objectPath = gitPath;
	objectPath.append("objects");
	boost::filesystem::create_directory(objectPath);

	std::ofstream indexFile("index");
	indexFile.close();

	boost::filesystem::current_path(testPath);

	// just Add -- that is it
	REQUIRE(Add(fileName) == AddErrorCode::NoError);

	// add index file
	boost::filesystem::current_path(gitPath);
	std::ifstream finalIndexFile("index");

	std::string insideIndex{ std::istreambuf_iterator<char>(finalIndexFile),
							 std::istreambuf_iterator<char>() };

	// check content -- empty
	finalIndexFile.close();

	REQUIRE(insideIndex == targetIndexContent);

	boost::filesystem::current_path(path); // return to original
	boost::filesystem::remove_all(testPath);
}