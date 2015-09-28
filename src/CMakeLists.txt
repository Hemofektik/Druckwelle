project (DruckwelleWMS)
file(GLOB_RECURSE SOURCES *.cpp *.h *.hpp *.c)

foreach(f ${SOURCES})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${CMAKE_CURRENT_SOURCE_DIR} ${f})
    set(SRCGR "Sources/${SRCGR}")

    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

add_definitions( -DPOCO_NO_AUTOMATIC_LIBS )

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /openmp")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /openmp")
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/3rdparty/cpplinq
	${CMAKE_SOURCE_DIR}/3rdparty/stb
	${CMAKE_SOURCE_DIR}/3rdparty/ZFXMath/include
)

link_libraries (	
	${CMAKE_SOURCE_DIR}/lib/zlibwapi.lib
	${CMAKE_SOURCE_DIR}/lib/gdal_i.lib
	${CMAKE_SOURCE_DIR}/lib/gsl.lib
	${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-dll.lib
	optimized ${CMAKE_SOURCE_DIR}/lib/PocoFoundation.lib
	optimized ${CMAKE_SOURCE_DIR}/lib/PocoNet.lib
	debug ${CMAKE_SOURCE_DIR}/lib/PocoFoundationd.lib
	debug ${CMAKE_SOURCE_DIR}/lib/PocoNetd.lib
)



add_executable (
   druckwelle
#   WIN32 # Only if you don't want the DOS prompt to appear in the background in Windows
#   MACOSX_BUNDLE
   ${SOURCES} # We could've listed the source files here directly instead of using a variable to store them
   #${INCLUDES}
)