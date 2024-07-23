cmake_minimum_required(VERSION 3.15)
project(LotusEngine C CXX)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include/assimp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/PerlinNoise)
set(THIRD_PARTY_LIBRARIES glfw glad ${OPENGL_LIBRARIES} ImGui assimp stb)

set(LOTUS_INCLUDE_DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Header generated automatically by CMake
configure_file(CMakeConfigFiles/path_manager_renderer.h.in ${CMAKE_CURRENT_SOURCE_DIR}/source/util/path_manager.h)

add_subdirectory(source)
add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(third_party)