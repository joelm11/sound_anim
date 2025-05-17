#version 330 core
layout(location = 0) in vec3 a_position;

// Transformation matrices (Must be uniforms)
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

const float kFreq = 1.0;

void main() {
    vec3 basePos = a_position;
    float height = sin(2 * 3.14159 * kFreq * u_time + basePos.z);
    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(basePos.x, basePos.y + height, basePos.z, 1.0);
}