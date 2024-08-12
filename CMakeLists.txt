cmake_minimum_required(VERSION 3.22)

project(FISK_TOOLS)

option(FISK_BUILD_TESTING "Build test executables" ON)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(tools)
add_subdirectory(examples)

Include(CTest)
Include(FetchContent)

if(FISK_BUILD_TESTING)
	FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        v3.0.1 # or a later release
	)

	FetchContent_MakeAvailable(Catch2)

	add_subdirectory(tools_test)
endif()