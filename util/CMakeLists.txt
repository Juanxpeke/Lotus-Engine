add_library(util
  include/file_manager.h
  src/file_manager.cpp)

set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(util PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)