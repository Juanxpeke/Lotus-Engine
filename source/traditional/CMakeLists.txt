set(RENDERER_NAME
    traditional_renderer)

set(RENDERER_HEADERS
    diffuse_flat_material.h
    diffuse_textured_material.h
    material.h
    mesh.h
    mesh_manager.h
    shader_program.h
    texture.h)

set(RENDERER_SOURCES
    material.cpp
    mesh.cpp
    mesh_manager.cpp
    renderer.cpp
    shader_program.cpp
    texture.cpp)

add_executable(${RENDERER_NAME} ${RENDERER_SOURCES} ${RENDERER_HEADERS} ${SCENE_SOURCES} ${SCENE_HEADERS} ${UTIL_HEADERS})

target_include_directories(${RENDERER_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${RENDERER_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${RENDERER_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${RENDERER_NAME} PROPERTIES FOLDER "renderers")