ADD_MODULE(helloWorld)

IF(${USE_CTEMPLATE})
	ADD_MODULE(advanced)
ENDIF()

ADD_MODULE(headersTest)

IF(${USE_REST})
	ADD_MODULE(rest)
	if(${USE_APACHE})
		target_link_libraries(mod_rest sqlite3)	
	endif()
	if(${USE_MONGOOSE})
		target_link_libraries(rest sqlite3)	
	endif()
ENDIF()

IF(${USE_MONGOOSE})
	add_executable(examples examples.cc)
	target_link_libraries(examples sqlite3 rikitiki)
ENDIF()
