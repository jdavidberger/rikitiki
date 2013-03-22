cmake_minimum_required (VERSION 2.6) 
project (rikitiki) 

SET(rikitiki_VERSION_MAJOR 0.1)

EXEC_PROGRAM(hg ARGS tip OUTPUT_VARIABLE hg_tip)
SET(rikitiki_minor_version "unknown")
STRING(REGEX REPLACE "changeset:[ \t]*([0-9]+):(.*)" "\\1" rikitiki_VERSION_MINOR "${hg_tip}")
FILE(WRITE ${CMAKE_BINARY_DIR}/VERSION "v${rikitiki_VERSION_MAJOR}.${rikitiki_VERSION_MINOR}")
FILE(READ ${CMAKE_BINARY_DIR}/VERSION rikitiki_version)
message("Building ${rikitiki_version}")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11 -Wno-reorder -O3")

# Not super useful, except for self documentation purposes
MACRO( disable_feature feature_)
       STRING(TOUPPER ${feature_} feature)
       SET(USE_${feature} 0) 
ENDMACRO(disable_feature)

MACRO( enable_feature feature_)
       STRING(TOUPPER ${feature_} feature)       
       SET("USE_${feature}" 1)
       SET(DOXYFILE_DEFINED "${DOXYFILE_DEFINED} RT_USE_${feature}")
       SET(CONFIG 
"#define RT_USE_${feature} 1
${CONFIG}")

ENDMACRO(enable_feature)

ENABLE_FEATURE(apache)
ENABLE_FEATURE(mongoose)
ENABLE_FEATURE(ctemplate)
ENABLE_FEATURE(jsoncpp)
ENABLE_FEATURE(rest)
ENABLE_FEATURE(configuration)
ENABLE_FEATURE(log)

DISABLE_FEATURE(stencet) # Not ready yet

set(MAKE_EXAMPLES 1)

include_directories(include)

# Might need some of these...
add_subdirectory(third)
include_directories(third/mxcomp/include)
include_directories(third/stencet/include)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(Doxygen)


add_custom_target(memcheck
	COMMAND GLIBCXX_FORCE_NEW=1 valgrind --tool=memcheck --leak-check=full --log-file=log ./examples
	COMMENT "Started example server via valgrind" VERBATIM
)

add_custom_target(helgrind
	COMMAND valgrind --tool=helgrind --log-file=log ./examples
	COMMENT "Started example server via valgrind" VERBATIM
)

add_custom_target(release
	COMMAND hg archive -t tgz ${CMAKE_BINARY_DIR}/rikitiki-${rikitiki_version}-src.tar.gz
	COMMENT "Generating release tar" VERBATIM
)

export(PACKAGE rikitiki)

install(DIRECTORY include/rikitiki DESTINATION include)
