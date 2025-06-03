add_library(uniforms STATIC src/uniforms.cc)

target_link_libraries(uniforms PRIVATE shader_m glm jimgui)