ADD_MODULE(helloWorld)

IF(${USE_CTEMPLATE})
	ADD_MODULE(advanced)
ENDIF()

ADD_MODULE(headersTest)
