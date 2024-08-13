
list(APPEND files main.cpp)

add_executable(tools_webpage ${files})

target_link_libraries(tools_webpage PUBLIC fisk_tools)
