add_executable(gittests 
    unittests.cpp
	InitUnitTests.cpp
	AddUnitTest
	catch.hpp
)


target_link_libraries(gittests
    PRIVATE
    git
	${Boost_LIBRARIES}
)

target_include_directories(gittests
    PRIVATE
    git
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	${Boost_INCLUDE_DIRS}
)
