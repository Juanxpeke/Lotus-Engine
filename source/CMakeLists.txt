set(UTIL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/path_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util/assimp_transformations.h)

set(SCENE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/transform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/node_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/camera.h)

set(SCENE_SOURCES )

add_subdirectory(traditional)