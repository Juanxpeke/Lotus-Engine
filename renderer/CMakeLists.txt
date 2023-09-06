add_executable(renderer 
  src/renderer.cpp
  src/shader_program.cpp
  src/shader_program.h)

set_property(TARGET renderer PROPERTY CXX_STANDARD 20)

target_link_libraries(renderer PRIVATE ${THIRD_PARTY_LIBRARIES})
target_include_directories(renderer PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/include)