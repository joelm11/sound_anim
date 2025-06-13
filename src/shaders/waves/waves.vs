#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

out vec3 FragPosW;
out vec3 FragNormal;

#define N_SINES 1
#define PI 3.14159

vec3 displacement(vec3 vert) {
    float amp = 1.0;
    float speed = 1.0;
    float freq = 1.0;
    vec2 dir = vec2(1.0, 0.0);
    float h = amp * sin(dot(dir, vert.xz) * 2 * PI * freq + u_time * speed * 2 * PI * freq);
    return vec3(vert.x, h, vert.z);
}

void main() {
    vec3 displacedPos = displacement(a_position);

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0);
}