add_library(shader_m STATIC shader_m.cc)
target_link_libraries(shader_m PRIVATE glm glad)
target_include_directories(shader_m PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

# New shader library
add_library(shader INTERFACE shader.hh uniform.hh)
target_include_directories(shader INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shader INTERFACE glm glad mesh uniforms stb)

# Shader implementations
add_library(skybox-shader STATIC skybox/skybox.hh skybox/skybox.cc)
target_link_libraries(skybox-shader PUBLIC shader)