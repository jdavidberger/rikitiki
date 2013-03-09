find_package(Doxygen)
if(DOXYGEN_FOUND)				
	SET(bg_color "#181a26")
	SET(fg_color "#dcdcdc")

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/header.i ${CMAKE_CURRENT_BINARY_DIR}/header.html)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/footer.i ${CMAKE_CURRENT_BINARY_DIR}/footer.html)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/style.i ${CMAKE_CURRENT_BINARY_DIR}/style.css)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/custom.i ${CMAKE_BINARY_DIR}/doc/html/custom.css)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.xml ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target(doc
		COMMAND	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		COMMAND	tar -cf ${CMAKE_BINARY_DIR}/rikitiki-${rikitiki_version}-doc.tar.gz -C ${CMAKE_BINARY_DIR}/doc .
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
