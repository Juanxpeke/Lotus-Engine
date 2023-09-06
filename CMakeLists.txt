cmake_minimum_required(VERSION 3.15)
project(opengl-rendering-engine C CXX)

add_definitions(-D_USE_MATH_DEFINES)

set(THIRD_PARTY_INCLUDE_DIRECTORIES 
  ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)
set(THIRD_PARTY_LIBRARIES glfw glad)

add_subdirectory(external)
add_subdirectory(renderer)