list(APPEND files "Concepts.h")
list(APPEND files "Singleton.h")
list(APPEND files "PlatformDetection.h")
list(APPEND files "DataProcessor.h")
list(APPEND files "ObjectPool.h")
list(APPEND files "Iterators.h")
list(APPEND files "Macros.h")
list(APPEND files "LockedResource.h")
list(APPEND files "Utility.h")
list(APPEND files "MathVector.h")
list(APPEND files "Matrix.h")
list(APPEND files "Shapes.h")

list(APPEND files "Json.h" 				"Json.cpp")
list(APPEND files "File.h" 				"File.cpp")
list(APPEND files "Stream.cpp" 			"Stream.h")
list(APPEND files "StreamWriter.cpp" 	"StreamWriter.h")
list(APPEND files "StreamReader.cpp" 	"StreamReader.h")
list(APPEND files "SystemValues.cpp" 	"SystemValues.h")
list(APPEND files "Event.h" 			"Event.cpp")
list(APPEND files "Logger.cpp" 			"Logger.h")
list(APPEND files "Time.cpp" 			"Time.h")
list(APPEND files "EggClock.cpp" 		"EggClock.h")

list(APPEND files "Net.h"				"WinNet.cpp"				"LinuxNet.cpp")
list(APPEND files "Socket.h"			"WinSocket.cpp"				"LinuxSocket.cpp")
list(APPEND files "TCPListenSocket.h"	"WinTCPListenSocket.cpp"	"LinuxTCPListenSocket.cpp")
list(APPEND files "TCPSocket.h"			"WinTCPSocket.cpp"			"LinuxTCPSocket.cpp")

add_library(fisk_tools ${files})

target_include_directories(fisk_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(WIN32)
	target_link_libraries(fisk_tools ws2_32.lib)
endif()