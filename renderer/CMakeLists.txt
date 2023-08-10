add_executable(renderer 
  src/renderer.cpp)

set_property(TARGET renderer PROPERTY CXX_STANDARD 20)

target_link_libraries(renderer PRIVATE ${THIRD_PARTY_LIBRARIES} ${UTIL_LIBRARY})
target_include_directories(renderer PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${UTIL_INCLUDE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/include)