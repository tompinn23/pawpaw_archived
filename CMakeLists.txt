cmake_minimum_required(VERSION 3.18)
project(mach)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
if(APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

find_package(OpenGL REQUIRED)


add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/glm)


add_executable(mach src/main.cxx src/window.cxx "src/BasicChunkRenderer.cxx")
target_link_libraries(mach glfw glad glm)