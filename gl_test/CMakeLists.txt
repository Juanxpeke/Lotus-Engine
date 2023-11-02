set(COMPUTE_SHADERS_HEADERS
    path_manager.h
    gl_utils.h)

set(COMPUTE_SHADERS_SOURCES
    gl_utils.cpp
    compute_shaders.cpp)

add_executable(compute_shaders ${COMPUTE_SHADERS_SOURCES} ${COMPUTE_SHADERS_HEADERS})

target_include_directories(compute_shaders PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
target_link_libraries(compute_shaders PRIVATE ${THIRD_PARTY_LIBRARIES})

set_property(TARGET compute_shaders PROPERTY CXX_STANDARD 20)