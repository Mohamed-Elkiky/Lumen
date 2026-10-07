cmake_minimum_required(VERSION 3.20)

project(Lumen VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release: debug renders are painfully slow
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(lumen
  src/main.cpp
  src/camera.cpp
  src/image.cpp
  src/stb_impl.cpp
)

# stb uses sprintf etc., silence MSVC's "unsafe function" warnings for that file only
set_source_files_properties(src/stb_impl.cpp PROPERTIES COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)

target_include_directories(lumen PRIVATE ${PROJECT_SOURCE_DIR}/include)
# SYSTEM hides warnings from third-party headers
target_include_directories(lumen SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/external)

target_compile_definitions(lumen PRIVATE
  LUMEN_VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

if(MSVC)
  target_compile_options(lumen PRIVATE /W4 /permissive- $<$<CONFIG:Release>:/O2>)
else()
  target_compile_options(lumen PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:Release>:-O3>)
endif()

# ---------- Tests ----------
enable_testing()

add_executable(test_vec3 tests/test_vec3.cpp)
target_include_directories(test_vec3 PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_test(NAME vec3 COMMAND test_vec3)