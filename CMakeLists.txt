cmake_minimum_required(VERSION 3.15)
project(gpu-driven-renderer C CXX)

add_definitions(-D_USE_MATH_DEFINES)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(THIRD_PARTY_INCLUDE_DIRECTORIES 
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)
set(THIRD_PARTY_LIBRARIES glfw glad ${OPENGL_LIBRARIES})

# Header generated automatically by CMake
configure_file(CMakeConfigFiles/path_manager_gl_tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/gl_tests/path_manager.h)
configure_file(CMakeConfigFiles/path_manager_renderer.h.in ${CMAKE_CURRENT_SOURCE_DIR}/renderer/path_manager.h)


add_subdirectory(third_party)
add_subdirectory(gl_tests)
add_subdirectory(renderer)