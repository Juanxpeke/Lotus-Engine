set(RENDERER_NAME
    LotusEngine)

set(RENDERER_HEADERS
    mesh.h
    mesh_manager.h
    texture.h
    texture_manager.h
    shader_program.h
    material.h
    diffuse_flat_material.h
    diffuse_textured_material.h
    mesh_instance.h
    renderer.h)

set(RENDERER_SOURCES
    mesh.cpp
    mesh_manager.cpp
    texture.cpp
    texture_manager.cpp
    shader_program.cpp
    material.cpp
    renderer.cpp)


add_library(${RENDERER_NAME} STATIC ${RENDERER_SOURCES} ${RENDERER_HEADERS} ${SCENE_SOURCES} ${SCENE_HEADERS} ${UTIL_HEADERS})

target_include_directories(${RENDERER_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${RENDERER_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${RENDERER_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${RENDERER_NAME} PROPERTIES FOLDER "renderers")


set(SAMPLE_NAME
    sample)

add_executable(${SAMPLE_NAME} sample.cpp ${RENDERER_SOURCES} ${RENDERER_HEADERS} ${SCENE_SOURCES} ${SCENE_HEADERS} ${UTIL_HEADERS})

target_include_directories(${SAMPLE_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${SAMPLE_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${SAMPLE_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${SAMPLE_NAME} PROPERTIES FOLDER "samples")