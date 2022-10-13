list(APPEND files "Concepts.h")

add_library(fisk_tools ${files})

target_include_directories(fisk_tools PUBLIC ${FISK_INCLUDE_DIRS})