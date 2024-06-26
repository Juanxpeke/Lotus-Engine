set(INSTANCED_RENDERER_NAME
    LotusRendererInstanced)

set(INSTANCED_RENDERER_HEADERS
    i_mesh.h
    i_mesh_manager.h
    i_texture.h
    i_texture_manager.h
    i_shader_program.h
    i_material.h
    i_diffuse_flat_material.h
    i_diffuse_textured_material.h
    graphics_batch.h
    i_mesh_instance.h
    i_renderer.h)

set(INSTANCED_RENDERER_SOURCES
    i_mesh.cpp
    i_mesh_manager.cpp
    i_texture.cpp
    i_texture_manager.cpp
    i_shader_program.cpp
    i_mesh_instance.cpp
    graphics_batch.cpp
    i_renderer.cpp)


add_library(${INSTANCED_RENDERER_NAME} STATIC ${INSTANCED_RENDERER_SOURCES} ${INSTANCED_RENDERER_HEADERS} ${SCENE_SOURCES} ${SCENE_HEADERS} ${UTIL_HEADERS})

target_include_directories(${INSTANCED_RENDERER_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${INSTANCED_RENDERER_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${INSTANCED_RENDERER_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${INSTANCED_RENDERER_NAME} PROPERTIES FOLDER "renderers")