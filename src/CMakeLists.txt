include_directories(
			${CMAKE_SOURCE_DIR}/src
			${THIRD_PARTY_DIR}/curl-7.28.1/include
		)

link_directories(
		${THIRD_PARTY_DIR}/curl-7.28.1/bin/lib
		${THIRD_PARTY_DIR}/ctemplate-2.2
	)


add_subdirectory(utils)

set (RT_SRC
    server.cc 
    connContext.cc 
    content_types.cc 
    routing.cc
)

set (RT_LIBS
    dl pthread curl utils
)

IF(${USE_MONGOOSE})
	add_subdirectory(mongoose)
ENDIF()

IF(${USE_CTEMPLATE})
	include_directories(${THIRD_PARTY_DIR}/ctemplate-2.2/src)
	add_subdirectory(ctemplate)
ENDIF()

IF(${USE_APACHE})
	include_directories(/usr/include/apr-1/)	
	add_subdirectory(apache)
ENDIF()

add_library(rikitiki ${RT_SRC})
target_link_libraries(rikitiki ${RT_LIBS})		     

if(${MAKE_EXAMPLES})
	add_subdirectory(examples)
ENDIF()


