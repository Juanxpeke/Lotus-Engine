define_property(
  TARGET
  PROPERTY FOLDER
  INHERITED
  BRIEF_DOCS "Set the folder name."
  FULL_DOCS  "Use to organize targets in an IDE.")

function(add_subdirectory_with_folder FOLDER_NAME SUBDIRECTORY_NAME)
    add_subdirectory(${SUBDIRECTORY_NAME} ${ARGN})
    set_property(DIRECTORY ${SUBDIRECTORY_NAME} PROPERTY FOLDER ${FOLDER_NAME})
endfunction(add_subdirectory_with_folder)

add_subdirectory_with_folder(third_party glad)
add_subdirectory_with_folder(third_party glfw)
add_subdirectory_with_folder(third_party/assimp assimp)
add_subdirectory_with_folder(third_party stb)

file(GLOB IMGUI_SOURCES imgui/*.cpp imgui/*.h)

add_library(ImGui STATIC
  ${IMGUI_SOURCES}
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(ImGui PUBLIC imgui imgui/backends)
target_link_libraries(ImGui PRIVATE glfw)

set_property(TARGET ImGui PROPERTY FOLDER third_party)

#file(GLOB imguifiles "imgui*.cpp" "*.h")
#add_library(ImGui STATIC ${imguifiles})
#target_include_directories(ImGui PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})