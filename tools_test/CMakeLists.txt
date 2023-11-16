list(APPEND files "Container.cpp")
list(APPEND files "Data.cpp")
list(APPEND files "Network.cpp")
list(APPEND files "Events.cpp")
list(APPEND files "Math.cpp")

add_executable(fisk_tools_test ${files})

target_include_directories(fisk_tools_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(fisk_tools_test PUBLIC fisk_tools)
target_link_libraries(fisk_tools_test PRIVATE Catch2::Catch2WithMain)


include(Catch)
catch_discover_tests(fisk_tools_test)