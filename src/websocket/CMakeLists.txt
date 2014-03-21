set(RT_SRC ${RT_SRC} 
	  websocket/websocketServer.cc 
	  websocket/websocketProcess.cc 
	  websocket/websocketContext.cc 
	   PARENT_SCOPE)

IF(NOT ${WIN32}) 	   
	set(RT_LIBS ${RT_LIBS} 		   
		dl
	PARENT_SCOPE)
ENDIF()
