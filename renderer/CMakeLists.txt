set(RENDERER_HEADERS
    assimp_transformations.h
    path_manager.h
    camera.h
    shader_program.h
    material.h
    diffuse_flat_material.h
    mesh.h)

set(RENDERER_SOURCES
    renderer.cpp
    camera.cpp
    shader_program.cpp
    material.cpp
    mesh.cpp)

# Generate a list of files that match the given path and store it into the ${shaders} variable
file(GLOB_RECURSE shaders  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)
# Define a grouping for source files in IDE project generation
source_group("Shaders" FILES ${shaders})

add_executable(renderer ${RENDERER_SOURCES} ${RENDERER_HEADERS} ${shaders})

target_include_directories(renderer PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(renderer PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET renderer PROPERTY CXX_STANDARD 20)

# Keep the structure of source groups analogically to the actual files and directories structure in the project
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERER_SOURCES} ${RENDERER_HEADERS})
