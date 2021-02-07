cmake_minimum_required(VERSION 3.17.3)

project(
  ShadesOfRay
  VERSION 0.1
  DESCRIPTION "Explorations in ray casting/tracing"
  LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17 -stdlib=libc++")

set(CLANG_DEFAULT_CXX_STDLIB )

set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")
set(CATCH_DIR "${EXTERNAL_DIR}/Catch2")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

add_subdirectory("${EXTERNAL_DIR}")
add_subdirectory("${SOURCE_DIR}")

include(CTest)
include(Catch)

add_subdirectory(test)


