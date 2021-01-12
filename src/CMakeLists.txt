cmake_minimum_required(VERSION 3.17.3)

set(EXTERNAL_DIR PRIVATE "${PROJECT_SOURCE_DIR}/external")
set(MATH_DIR PRIVATE "${PROJECT_SOURCE_DIR}/external")
set(UTIL_DIR PRIVATE "${PROJECT_SOURCE_DIR}/util")

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# add the rendering library 
add_subdirectory(renderer)

add_executable(stub stub.cpp)
target_include_directories(stub PRIVATE ${EXTERNAL_DIR})

# TODO(mjb): make this optional
target_compile_definitions(stub PRIVATE NDEBUG)

target_compile_features(stub PRIVATE cxx_std_17)

include_directories(stub ${SDL2_INCLUDE_DIRS})

target_link_libraries(stub renderer)