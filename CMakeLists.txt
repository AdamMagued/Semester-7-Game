cmake_minimum_required(VERSION 3.10)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw-3.5.1)
add_subdirectory(external/glm)

find_package(OpenGL REQUIRED)

add_executable(MyGame
    src/main.cpp
    src/ui/Hud.cpp
    src/renderer/Objects.cpp
    src/renderer/Sprites.cpp
    src/renderer/Drawing.cpp
    src/game/Gameplay.cpp
    src/game/CutsceneDrawing.cpp
    src/game/conversation_cutscene.cpp
)

target_include_directories(MyGame PRIVATE
    external/glad/include
    src
)

target_link_libraries(MyGame PRIVATE
    glfw
    OpenGL::GL
    glm::glm
)
