include_directories(	
			${CMAKE_SOURCE_DIR}/src
			${THIRD_PARTY_DIR}/curl-7.28.1/include
			${THIRD_PARTY_DIR}/ctemplate-2.2/src
			/usr/include/apr-1/
		)

link_directories(
		${THIRD_PARTY_DIR}/curl-7.28.1/bin/lib
		${THIRD_PARTY_DIR}/ctemplate-2.2
	)


add_subdirectory(utils)

add_definitions(-DCTEMPLATE_ROOT_DIRECTORY="${CMAKE_SOURCE_DIR}/html/ctemplate")
add_library(rikitiki 
		   server.cc 
		   connContext.cc 
		   content_types.cc 
		   routing.cc
		   apache/connContext.cc
		   mongoose/connContext.cc 
		   mongoose/server.cc
		   mongoose/mongoose.c
		   config.cc 
		   ctemplate/connContext_ext.cc
		   )
target_link_libraries(rikitiki ctemplate dl pthread curl utils config++)

add_subdirectory(examples)
