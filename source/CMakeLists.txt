set(UTIL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/path_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util/assimp_transformations.h)

set(SCENE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/camera.h)

set(SCENE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/camera.cpp)

add_subdirectory(traditional)