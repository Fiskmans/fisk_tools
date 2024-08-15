cmake_minimum_required(VERSION 3.22)

project(FISK_TOOLS)

option(FISK_BUILD_TESTING "Build test executables" ON)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

Include(FetchContent)

FetchContent_Declare(
  sha1
  GIT_REPOSITORY https://github.com/vog/sha1.git
  GIT_TAG        1.4
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(sha1)
if(NOT sha1_POPULATED)
  FetchContent_Populate(sha1)
endif()

add_library(sha1 INTERFACE)
target_include_directories(sha1 INTERFACE ${sha1_SOURCE_DIR})

add_subdirectory(tools)
add_subdirectory(examples)

Include(CTest)

if(FISK_BUILD_TESTING)
	FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        v3.0.1 # or a later release
	)

	FetchContent_MakeAvailable(Catch2)

	add_subdirectory(tools_test)
endif()