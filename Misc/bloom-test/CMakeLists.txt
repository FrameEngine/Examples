cmake_minimum_required(VERSION 3.14)
project(DemoProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  FrameEngine
  GIT_REPOSITORY    https://github.com/FrameEngine/FrameEngine.git
  GIT_TAG           rendering/lighting
)
FetchContent_MakeAvailable(FrameEngine)

file(GLOB_RECURSE DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(demo ${DEMO_SOURCES})

target_include_directories(demo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(demo PRIVATE FrameEngineLib)
