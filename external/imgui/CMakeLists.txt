add_library(jimgui STATIC
    imgui-src/imgui.cpp
    imgui-src/imgui_demo.cpp
    imgui-src/imgui_draw.cpp
    imgui-src/imgui_tables.cpp
    imgui-src/imgui_widgets.cpp
    src/jimgui.cc
)

# Set include directories for ImGui
target_include_directories(jimgui PUBLIC
    imgui-src
    imgui-src/backends
    src
)

# Add backend sources to the ImGui target
target_sources(jimgui PRIVATE
    imgui-src/backends/imgui_impl_glfw.cpp
    imgui-src/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(jimgui PRIVATE glfw glad)