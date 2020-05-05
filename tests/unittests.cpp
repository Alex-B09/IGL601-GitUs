#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"

#include <gitUtils.h>

#include <string>
#include <boost/filesystem.hpp>

TEST_CASE("Test separation args") 
{
	SECTION("Simple args")
	{
		std::string cmd0Args = "";
		std::string cmd3Args = "hello me today";
		std::string cmd7Args = "hello me today this is anoying brouuuu";

		REQUIRE(GitUtils::GetSeparatedArgs(cmd0Args).size() == 0);
		REQUIRE(GitUtils::GetSeparatedArgs(cmd3Args).size() == 3);
		REQUIRE(GitUtils::GetSeparatedArgs(cmd7Args).size() == 7);
	}

	SECTION("Specific args")
	{
		std::string cmd1 = "";
		std::string cmd2 = "--help";
		std::string cmd3 = "test1 \"avec des espaces\" --help truc1212";

		auto result1 = GitUtils::GetSeparatedArgs(cmd1);
		auto result2 = GitUtils::GetSeparatedArgs(cmd2);
		auto result3 = GitUtils::GetSeparatedArgs(cmd3);
		
		REQUIRE(result1.size() == 0);

		// result2
		REQUIRE(result2.size() == 1);
		if (result2.size() == 1)
		{
			REQUIRE(result2[0] == "--help");
		}

		// result3
		REQUIRE(result3.size() == 4);
		if (result3.size() == 4)
		{
			REQUIRE(result3[0] == "test1");
			REQUIRE(result3[1] == "avec des espaces");
			REQUIRE(result3[2] == "--help");
			REQUIRE(result3[3] == "truc1212");
		}
	}
}


TEST_CASE("command categorisation")
{
	std::string cmdCommit = "commit";
	std::string cmdInit = "init";
	std::string cmdAdd = "add";
	std::string cmdError = "brouasdfsdefs";
	std::string cmdError2 = "--help";
	std::string cmdError3 = "commit test1";
	std::string cmdError4 = "init test1";
	std::string cmdError5 = "add test1";

	REQUIRE(GitUtils::GetCmdCategory(cmdCommit) == GitUtils::cmdCategory::commit);
	REQUIRE(GitUtils::GetCmdCategory(cmdAdd) == GitUtils::cmdCategory::add);
	REQUIRE(GitUtils::GetCmdCategory(cmdInit) == GitUtils::cmdCategory::init);
	REQUIRE(GitUtils::GetCmdCategory(cmdError) == GitUtils::cmdCategory::error);
	REQUIRE(GitUtils::GetCmdCategory(cmdError2) == GitUtils::cmdCategory::error);
	REQUIRE(GitUtils::GetCmdCategory(cmdError3) == GitUtils::cmdCategory::error);
	REQUIRE(GitUtils::GetCmdCategory(cmdError4) == GitUtils::cmdCategory::error);
	REQUIRE(GitUtils::GetCmdCategory(cmdError5) == GitUtils::cmdCategory::error);
}

TEST_CASE("Help finder")
{
	std::vector<std::string> elems;

	SECTION("Only element")
	{
		elems.push_back("--help");

		REQUIRE(GitUtils::GetHelpArg(elems) == 0);
	}

	SECTION("Second position")
	{
		elems.push_back("brouu");
		elems.push_back("--help");
		elems.push_back("fdfs");
		elems.push_back("test");

		REQUIRE(GitUtils::GetHelpArg(elems) == 1);
	}

	SECTION("Is not supposed to be there")
	{
		elems.push_back("brouu");
		elems.push_back("fdfs");
		elems.push_back("test");
		elems.push_back("df");

		REQUIRE(GitUtils::GetHelpArg(elems) == -1);
	}

	SECTION("Last position")
	{
		elems.push_back("brouu");
		elems.push_back("fdfs");
		elems.push_back("test");
		elems.push_back("--help");

		REQUIRE(GitUtils::GetHelpArg(elems) == 3);
	}
}

TEST_CASE("Validation of git folder")
{
	auto path = boost::filesystem::current_path();
	auto testPath = path.append("testUtils");
	
	boost::filesystem::create_directory(testPath);
	boost::filesystem::current_path(testPath);

	SECTION("NonValid Path")
	{
		REQUIRE_FALSE(GitUtils::isValidGitFolder());
	}

	SECTION("Valid Path")
	{
		auto gitFolder = testPath;
		boost::filesystem::create_directory(gitFolder.append(".git"));
		REQUIRE(GitUtils::isValidGitFolder());
	}

	boost::filesystem::current_path(path.append("../")); // return to original
	boost::filesystem::remove_all(testPath);
}

// test index
TEST_CASE("IndexFile interactions")
{
	auto path = boost::filesystem::current_path();
	
	auto testPath = path;
	testPath.append("testUtilsIndex");
	boost::filesystem::create_directory(testPath);

	auto gitPath = testPath;
	gitPath.append(".git");
	boost::filesystem::create_directory(gitPath);

	boost::filesystem::current_path(gitPath);
	
	// add index file
	std::ofstream indexFile("index");

	// check content -- empty
	indexFile.close();
	boost::filesystem::current_path(testPath);

	// get index file
	auto indexContent = GitUtils::ReadIndexFile();
	REQUIRE(indexContent.size() == 0);

	// add elems to index file
	GitUtils::IndexEntry entry;
	entry.sha = { 0xa0000001, 0xa0000002, 0xa0000003, 0xa0000004, 0xa0000005 };
	entry.fileName = "blah1";
	indexContent.push_back(entry);

	GitUtils::IndexEntry entry2;
	entry2.sha = { 0xb0000001, 0xb0000002, 0xb0000003, 0xb0000004, 0xb0000005 };
	entry2.fileName = "blah2";

	indexContent.push_back(entry2);
	
	GitUtils::WriteToIndexFile(indexContent);

	// check index file
	std::ifstream indexFileInput(".git/index");

	std::string insideIndex{ std::istreambuf_iterator<char>(indexFileInput),
							 std::istreambuf_iterator<char>() };
	indexFileInput.close();

	std::string target = "2\na0000001a0000002a0000003a0000004a0000005 blah1\nb0000001b0000002b0000003b0000004b0000005 blah2\n";

	REQUIRE(insideIndex == target);

	boost::filesystem::current_path(path); // return to original
	boost::filesystem::remove_all(testPath);
}
