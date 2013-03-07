ADD_MODULE(helloWorld)

IF(${USE_CTEMPLATE})
	ADD_MODULE(advanced)
ENDIF()

ADD_MODULE(headersTest)
ADD_MODULE(rest)
target_link_libraries(mod_rest sqlite3)	
target_link_libraries(rest sqlite3)	

add_executable(examples examples.cc)
target_link_libraries(examples sqlite3 rikitiki)
