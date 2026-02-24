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

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1
)

FetchContent_Declare(
  GoogleBench
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.0
)

FetchContent_MakeAvailable(sha1)

add_subdirectory(sha1)
add_subdirectory(tools)
add_subdirectory(examples)

Include(CTest)

if(FISK_BUILD_TESTING)

	set(BENCHMARK_ENABLE_GTEST_TESTS false)
	
	FetchContent_MakeAvailable(Catch2)
	FetchContent_MakeAvailable(GoogleBench)

	add_subdirectory(tools_test)
endif()