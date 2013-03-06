cmake_minimum_required (VERSION 2.6) 
project (rikitiki) 
SET(rikitiki_major_version v0.1)

EXEC_PROGRAM(hg ARGS tip OUTPUT_VARIABLE hg_tip)
SET(rikitiki_minor_version "unknown")
STRING(REGEX REPLACE "changeset:[ \t]*([0-9]+):(.*)" "\\1" rikitiki_minor_version "${hg_tip}")

SET(rikitiki_version "${rikitiki_major_version}.${rikitiki_minor_version}")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11 -Wno-reorder")

set(USE_APACHE 1)
set(USE_MONGOOSE 1)

set(USE_CTEMPLATE 1)
set(MAKE_EXAMPLES 1)

IF(${USE_CTEMPLATE})
	ADD_DEFINITIONS(-DUSE_CTEMPLATE)
ENDIF()

IF(${USE_APACHE})
	ADD_DEFINITIONS(-DUSE_APACHE)
ENDIF()

IF(${USE_MONGOOSE})
	ADD_DEFINITIONS(-DUSE_MONGOOSE)
ENDIF()

add_subdirectory (src)


find_package(Doxygen)
if(DOXYGEN_FOUND)	
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
