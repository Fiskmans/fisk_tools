list(APPEND files "Concepts.h")
list(APPEND files "DataProcessor.h")
list(APPEND files "StreamWriter.cpp" "StreamWriter.h")
list(APPEND files "StreamReader.cpp" "StreamReader.h")


add_library(fisk_tools ${files})

set_property(TARGET fisk_tools PROPERTY CXX_STANDARD 20)
target_include_directories(fisk_tools PUBLIC ${FISK_INCLUDE_DIRS})