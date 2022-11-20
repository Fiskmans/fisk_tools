list(APPEND files "Concepts.h")
list(APPEND files "DataProcessor.h")
list(APPEND files "StreamWriter.cpp" "StreamWriter.h")
list(APPEND files "StreamReader.cpp" "StreamReader.h")
list(APPEND files "Event.h")
list(APPEND files "Logger.cpp" "Logger.h")
list(APPEND files "Time.cpp" "Time.h")
list(APPEND files "Macros.h")
list(APPEND files "LockedResource.h")


add_library(fisk_tools ${files})

set_property(TARGET fisk_tools PROPERTY CXX_STANDARD 20)
target_include_directories(fisk_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)