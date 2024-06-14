set(ENGINE_NAME
    LotusEngine)

set(ENGINE_HEADERS
    ../gpu_mesh.h
    mesh.h
    mesh_manager.h
    shader_program.h
    material.h
    diffuse_flat_material.h
    mesh_instance.h
    renderer.h
    ../../terrain/clipmap.h)

set(ENGINE_SOURCES
    ../gpu_mesh.cpp
    mesh.cpp
    mesh_manager.cpp
    shader_program.cpp
    renderer.cpp
    ../../terrain/clipmap.cpp)


add_library(${ENGINE_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${SCENE_HEADERS} ${LIGHTING_HEADERS} ${UTIL_HEADERS})

target_include_directories(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${ENGINE_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${ENGINE_NAME} PROPERTIES FOLDER "engine")