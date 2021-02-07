enable_testing()
add_executable(tests tests.cpp raytracer.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_include_directories(tests PRIVATE "${SOURCE_DIR}")

catch_discover_tests(tests)

message(${CMAKE_CXX_FLAGS})