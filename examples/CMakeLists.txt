function(add_example TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})

	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER example)
	
  target_link_libraries(${TARGETNAME} PRIVATE LotusEngine)
	target_include_directories(${TARGETNAME} PRIVATE ${LOTUS_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})

endfunction(add_example)

add_example(t_single_mesh_example t_single_mesh_example.cpp)
