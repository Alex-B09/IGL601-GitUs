add_library(git
    gitAdd.h 
    gitAdd.cpp
    gitUtils.h 
    gitUtils.cpp
    gitInit.h 
    gitInit.cpp
    gitCommit.h 
    gitCommit.cpp
)

target_link_libraries(git 
    PUBLIC 
		${Boost_LIBRARIES}
)
	
target_include_directories(git 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)