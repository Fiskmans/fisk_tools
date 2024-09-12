list(APPEND files "JSON.cpp")

add_executable(fisk_tools_benchmark ${files})

target_include_directories(fisk_tools_benchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_tools_benchmark PUBLIC fisk_tools)
target_link_libraries(fisk_tools_benchmark PRIVATE benchmark::benchmark_main)