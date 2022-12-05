cmake_minimum_required(VERSION 3.18)
project(mach)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD_REQUIRED 20)
if(APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
endif()

find_package(OpenGL REQUIRED)

add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/xz)
add_subdirectory(third_party/boost)

add_executable(mach src/main.cxx src/window.cxx src/BasicChunkRenderer.cxx src/Zip.cxx)
target_link_libraries(mach glfw glad glm boost_bits liblzma)


add_subdirectory(docs)