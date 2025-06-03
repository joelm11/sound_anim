cmake_minimum_required(VERSION 3.23)

project(joelgl LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# External libs
add_subdirectory(external/glfw-3.4)
add_subdirectory(external/glad)
add_subdirectory(external/stb)
add_subdirectory(external/glm-1.0.1)
add_subdirectory(external/imgui)

# Internal libs
add_subdirectory(src/lib)
add_subdirectory(src/init)

add_executable(window src/main.cpp src/shaders/shader_m.h src/shaders/shader_m.cc)
target_link_libraries(window 
    PUBLIC
        glfw
        glad
        stb
        glm
        jimgui
        init_routines
        mesh)