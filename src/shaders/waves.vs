#version 330 core
layout(location = 0) in vec3 a_position;

// Transformation matrices (Must be uniforms)
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}