cmake_minimum_required(VERSION 3.15)
project(gpu-driven-renderer C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GRAPHICS_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)
set(GRAPHICS_LIBRARIES glfw glad ${OPENGL_LIBRARIES})

set(THIRD_PARTY_INCLUDE_DIRECTORIES 
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include/assimp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
set(THIRD_PARTY_LIBRARIES glfw glad ${OPENGL_LIBRARIES} assimp stb)

# Header generated automatically by CMake
configure_file(CMakeConfigFiles/path_manager_renderer.h.in ${CMAKE_CURRENT_SOURCE_DIR}/source/path_manager.h)
configure_file(CMakeConfigFiles/path_manager_gl_tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/gl_tests/path_manager.h)

add_subdirectory(source)
add_subdirectory(gl_tests)
add_subdirectory(tests)
add_subdirectory(third_party)