set(SOURCE_FILES stub.cpp renderer.cpp image.cpp)

set(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)

find_package(SDL2 REQUIRED)

add_executable(stub ${SOURCE_FILES})
target_include_directories(stub PRIVATE ${EXTERNAL_DIR})
# target_compile_definitions(stub PRIVATE MJB_RENDERER_USE_SDL)
target_compile_features(stub PRIVATE cxx_std_17)

target_link_libraries(stub ${SDL2_LIBRARIES})
include_directories(stub ${SDL2_INCLUDE_DIRS})
