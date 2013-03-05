IF(${USE_MONGOOSE})
	add_executable(helloWorld helloWorld.cc)
	target_link_libraries(helloWorld rikitiki)

	add_executable(headers headers.cc)
	target_link_libraries(headers rikitiki)

	IF(${USE_CTEMPLATE})
		add_executable(advanced advanced.cc)
		target_link_libraries(advanced rikitiki)
	ENDIF()

	
ENDIF()

IF(${USE_APACHE})
	# These commands will create a module suitable to be loaded into apache
	# You will still neeed to copy the resulting file to the apache modules
	# folder with something like:
	# 'sudo cp lib/mod_helloWorld.so /etc/httpd/modules'
	# And then add the following line to /etc/httpd/conf/httpd.conf:
	# LoadModule helloWorld_module modules/mod_helloWorld.so
	add_library(mod_helloWorld SHARED mod_helloWorld.cc)
	set_target_properties(mod_helloWorld PROPERTIES PREFIX "")
	target_link_libraries(mod_helloWorld rikitiki)

	IF(${USE_CTEMPLATE})
		add_library(mod_advanced SHARED mod_advanced.cc)
		set_target_properties(mod_advanced PROPERTIES PREFIX "")
		target_link_libraries(mod_advanced rikitiki)
	ENDIF()
ENDIF()
