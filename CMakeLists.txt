cmake_minimum_required(VERSION 3.10)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw-3.5.1)
add_subdirectory(external/glm)

find_package(OpenGL REQUIRED)

# Automatically find all .cpp files in src/ and all its subdirectories
file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")

add_executable(MyGame ${GAME_SOURCES})

target_include_directories(MyGame PRIVATE
    external/glad/include
    src
)

target_link_libraries(MyGame PRIVATE
    glfw
    OpenGL::GL
    glm::glm
)
