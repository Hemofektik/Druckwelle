cmake_minimum_required(VERSION 2.8)

project (Druckwelle)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")

if(CMAKE_COMPILER_IS_GNUCXX)
	#set(CMAKE_BUILD_TYPE Debug)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_subdirectory(src)
add_subdirectory(test)

