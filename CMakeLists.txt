cmake_minimum_required(VERSION 3.22)

if(FISK_TOOLS_INCLUDED)
  return()
endif()
set(FISK_TOOLS_INCLUDED true)

project(FISK_TOOLS)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(tools)