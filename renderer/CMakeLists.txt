add_executable(renderer 
  src/renderer.cpp
  src/shader_program.cpp
  src/shader_program.h
  src/mesh.h
  src/mesh.cpp
  src/camera.h
  src/camera.cpp
  src/path_manager.h
  src/material.h
  src/material.cpp
  src/diffuse_flat_material.h)

set_property(TARGET renderer PROPERTY CXX_STANDARD 20)

target_link_libraries(renderer PRIVATE ${THIRD_PARTY_LIBRARIES})
target_include_directories(renderer PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/include)