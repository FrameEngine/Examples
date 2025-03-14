cmake_minimum_required(VERSION 3.14)
project(DemoProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  FrameEngine
  GIT_REPOSITORY    https://github.com/FrameEngine/FrameEngine.git
  GIT_TAG           main
)
FetchContent_MakeAvailable(FrameEngine)
add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE FrameEngineLib)

