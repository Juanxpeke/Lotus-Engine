function(configure_executable EXECUTABLE_NAME SOURCES)
    add_executable(${EXECUTABLE_NAME} ${SOURCES} gl_utils.cpp path_manager.h gl_utils.h)

    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 20)

    set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER "gl_tests")
endfunction()

configure_executable(raycast compute_shaders/raycast.cpp)
configure_executable(quads_draw_elements draw_calls/q_de.cpp)
configure_executable(quads_draw_elements_instanced draw_calls/q_de_ins.cpp)
configure_executable(quads_triangles_multi_draw_elements draw_calls/qt_mde.cpp)
configure_executable(quads_triangles_multi_draw_elements_indirect draw_calls/qt_mde_ind.cpp)
configure_executable(particles_draw_elements draw_calls/p_de.cpp)
configure_executable(particles_draw_elements_instanced draw_calls/p_de_ins.cpp)
configure_executable(particles_multi_draw_elements draw_calls/p_mde.cpp)
configure_executable(particles_multi_draw_elements_indirect draw_calls/p_mde_ind.cpp)
configure_executable(different_particles_multi_draw_elements_indirect draw_calls/pv_mde_ind.cpp)