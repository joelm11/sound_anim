add_library(shader_m STATIC shader_m.cc)
target_link_libraries(shader_m PRIVATE glm glad)
target_include_directories(shader_m PUBLIC "${CMAKE_LIST_CURRENT_DIR}")

# New shader library
add_library(shader INTERFACE shader.hh uniform.hh)
target_link_libraries(shader INTERFACE glm glad mesh uniforms)