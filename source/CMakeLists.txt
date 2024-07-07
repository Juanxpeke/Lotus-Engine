# Header files

set(UTIL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/util/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util/path_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util/assimp_transformations.h)

set(MATH_HEADERS
    )

set(SCENE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/transform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/node_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/camera.h)

set(LIGHTING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/lighting/directional_light.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lighting/point_light.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lighting/light_manager.h)

set(TERRAIN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/procedural_data_generator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/geoclipmap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/terrain_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/object_placer.h)

set(RENDER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/texture_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/rendering_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/mesh_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/diffuse_flat_material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/mesh_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/indirect_object_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/diffuse_flat_material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/diffuse_textured_material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/mesh_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/texture_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/renderer.h)

# Source files

set(LIGHTING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lighting/light_manager.cpp)

set(TERRAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/procedural_data_generator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/geoclipmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/terrain_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/object_placer.cpp)

set(RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/texture_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/rendering_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/mesh_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/indirect_object_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/material.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/mesh_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/texture_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/renderer.cpp)

set(ENGINE_NAME
    LotusEngine)

set(ENGINE_HEADERS
    ${UTIL_HEADERS}
    ${SCENE_HEADERS}
    ${LIGHTING_HEADERS}
    ${TERRAIN_HEADERS}
    ${RENDER_HEADERS})

set(ENGINE_SOURCES
    ${LIGHTING_SOURCES}
    ${TERRAIN_SOURCES}
    ${RENDER_SOURCES})

add_library(${ENGINE_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${ENGINE_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${ENGINE_NAME} PROPERTIES FOLDER "engine")