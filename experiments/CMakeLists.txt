function(add_experiment TARGET_NAME)
	add_executable(${TARGET_NAME} ${TARGET_NAME}.cpp)

	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER experiments)

	target_link_libraries(${TARGET_NAME} PRIVATE LotusEngine)
	target_include_directories(${TARGET_NAME} PRIVATE ${LOTUS_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
endfunction(add_experiment)

add_experiment(objects_experiment)
add_experiment(procedural_experiment)
