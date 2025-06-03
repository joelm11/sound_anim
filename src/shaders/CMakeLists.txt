add_library(shader_m STATIC shader_m.cc)
target_link_libraries(shader_m PRIVATE glm glad)
target_include_directories(shader_m PUBLIC "${CMAKE_LIST_CURRENT_DIR}")