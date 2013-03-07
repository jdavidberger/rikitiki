add_custom_target(test
	COMMAND python test-examples.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
	COMMENT "Running examples test" VERBATIM
)
