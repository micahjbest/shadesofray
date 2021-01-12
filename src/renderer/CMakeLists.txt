set(SOURCES image.cpp renderer.cpp)
set(HEADERS image.hpp pixel.hpp pixelbuffer.hpp renderer.hpp)


add_library(renderer "${SOURCES}" "${HEADERS}")
target_compile_features(renderer PRIVATE cxx_std_17)

target_include_directories(renderer PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(renderer PUBLIC "${MATH_DIR}")
target_include_directories(renderer PUBLIC "${UTIL_DIR}")

target_link_libraries(renderer ${SDL2_LIBRARIES})
target_link_libraries(renderer ${OPENGL_gl_LIBRARY})
target_link_libraries(renderer imgui)
