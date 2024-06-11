function(add_visual_test TARGET_NAME)
	add_executable(${TARGET_NAME} ${TARGET_NAME}.cpp)

	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER tests)

	target_link_libraries(${TARGET_NAME} PRIVATE LotusEngine)
	target_include_directories(${TARGET_NAME} PRIVATE ${LOTUS_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})
endfunction(add_indirect_example)

add_visual_test(different_meshes)
add_visual_test(different_materials)
#add_visual_test(different_shaders)

add_visual_test(change_transforms)
add_visual_test(change_meshes)
add_visual_test(change_material_values)

add_visual_test(random_generation)

add_visual_test(terrain)