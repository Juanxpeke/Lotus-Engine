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
    ${CMAKE_CURRENT_SOURCE_DIR}/lighting/point_light.h)

set(TERRAIN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/procedural_data_generator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/geoclipmap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/terrain_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/object_placer.h)

set(RENDER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/render/mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/mesh_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/diffuse_flat_material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/mesh_object.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/texture_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/rendering_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/traditional_object_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/indirect_object_renderer.h)

set(APPLICATION_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering_application.h)

# Source files

set(LIGHTING_SOURCES
    )

set(TERRAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/procedural_data_generator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/geoclipmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/terrain_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/terrain/object_placer.cpp)

set(RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/render/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/mesh_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/gpu_texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/texture_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/rendering_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/traditional/traditional_object_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/indirect/indirect_object_renderer.cpp)

set(APPLICATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering_application.cpp)

# Engine

set(ENGINE_NAME
    LotusEngine)

set(ENGINE_HEADERS
    ${UTIL_HEADERS}
    ${SCENE_HEADERS}
    ${LIGHTING_HEADERS}
    ${TERRAIN_HEADERS}
    ${RENDER_HEADERS}
    ${APPLICATION_HEADERS})

set(ENGINE_SOURCES
    ${LIGHTING_SOURCES}
    ${TERRAIN_SOURCES}
    ${RENDER_SOURCES}
    ${APPLICATION_SOURCES})

add_library(${ENGINE_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(${ENGINE_NAME} PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET ${ENGINE_NAME} PROPERTY CXX_STANDARD 20)
set_target_properties(${ENGINE_NAME} PROPERTIES FOLDER "engine")