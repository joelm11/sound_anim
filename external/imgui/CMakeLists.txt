# Copy required source and header files for GLFW OpenGL context
set(IMGUI_SRC_FILES
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/imgui.h"
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/imconfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/backends/imgui_impl_glfw.h"
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/backends/imgui_impl_opengl3.h"
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/imgui-src/backends/imgui_impl_opengl3.cpp"
    )

set(jimgui_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# --- Custom Target to run the copy command every time, before anything else ---
add_custom_target(PreBuildCopyFiles ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${IMGUI_SRC_FILES} "${jimgui_SRC_DIR}"
    COMMENT "Copying specified files to ${jimgui_SRC_DIR} before any build target"
)

file(GLOB_RECURSE STATIC_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(jimgui STATIC ${STATIC_LIB_SOURCES})
target_include_directories(jimgui PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src" "${CMAKE_CURRENT_LIST_DIR}/imgui-src/backends")
target_link_libraries(jimgui PUBLIC glfw)
