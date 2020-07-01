cmake_minimum_required(VERSION 3.17.3)

find_package(SDL2 REQUIRED)

add_library(
  imgui
  # Main Imgui files
  imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
  # SDL2+OpenGL-specific files
  imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl3.cpp)

# Make SDL2 available to Imgui
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})
# imgui/exapmles/ contains the sdl implementation
target_include_directories(imgui PUBLIC imgui/ imgui/examples/)
# Configure Imgui to use OpengGL through SDL2
target_compile_definitions(
  imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL_opengl.h>
               GL_GLEXT_PROTOTYPES=1)
