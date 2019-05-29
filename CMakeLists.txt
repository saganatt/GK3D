cmake_minimum_required(VERSION 3.13)
project(Lab_4)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        Lab_4

        src/glad.c
        inc/stb_image.cpp
        inc/tiny_obj_loader.cpp

        src/main.cpp

        src/objects/Camera.cpp
        src/objects/Player.cpp
        src/objects/Entity.cpp
        src/objects/Terrain.cpp

        src/renderers/EntityRenderer.cpp
        src/renderers/TerrainRenderer.cpp
        src/renderers/SkyboxRenderer.cpp
        src/shaders/EntityShader.cpp
        src/shaders/TerrainShader.cpp
        src/shaders/SkyboxShader.cpp

        src/utils/GameTime.cpp
        src/utils/FrameBuffer.cpp
        src/utils/Loader.cpp
        src/utils/Model.cpp
        src/utils/ShaderProgram.cpp
)

include_directories(inc)

target_link_libraries(Lab_4 glfw3 X11 dl pthread) # GL GLU Xxf86vm Xrandr Xi Xinerama Xcursor