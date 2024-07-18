
list(APPEND files main.cpp)

add_executable(tools_sniffer ${files})

target_link_libraries(tools_sniffer PUBLIC fisk_tools)
