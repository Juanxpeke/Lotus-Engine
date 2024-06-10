function(add_example TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})

	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER examples)
	
  target_link_libraries(${TARGETNAME} PRIVATE LotusRenderer)
	target_include_directories(${TARGETNAME} PRIVATE ${LOTUS_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})

endfunction(add_example)

function(add_indirect_example TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})

	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER examples)

	target_link_libraries(${TARGETNAME} PRIVATE LotusEngine)
	target_include_directories(${TARGETNAME} PRIVATE ${LOTUS_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
endfunction(add_indirect_example)


add_example(basic_example basic_example.cpp)
add_example(single_mesh_example single_mesh_example.cpp)
add_example(same_mesh_different_material same_mesh_different_material.cpp)
add_example(primitives primitives_example.cpp)

add_indirect_example(indirect_example indirect_example.cpp)
add_indirect_example(indirecT_basic_example ind_basic_example.cpp)