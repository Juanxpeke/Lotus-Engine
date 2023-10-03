cmake_minimum_required(VERSION 3.15)
project(opengl-rendering-engine C CXX)

add_definitions(-D_USE_MATH_DEFINES)

set(THIRD_PARTY_INCLUDE_DIRECTORIES 
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)
set(THIRD_PARTY_LIBRARIES glfw glad ${OPENGL_LIBRARIES})

# Header generated automatically by CMake
configure_file(CMakeConfigFiles/path_manager.h.in ${CMAKE_CURRENT_SOURCE_DIR}/renderer/path_manager.h)

add_subdirectory(third_party)
add_subdirectory(renderer)