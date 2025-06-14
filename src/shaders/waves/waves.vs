#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

#define NUMWAVES 4
uniform float u_amps[NUMWAVES];
uniform float u_freqs[NUMWAVES];
uniform vec2 u_dirs[NUMWAVES];

out vec3 FragPosW;
out vec3 FragNormal;

#define PI 3.14159
const float kSpeed = 0.5;

float displacement(vec3 vert) {
    float h = 0.0;
    for(int i = 0; i < NUMWAVES; ++i) {
        float phase = dot(u_dirs[i], vert.xz) * 2.0 * PI * u_freqs[i] + u_time * kSpeed * 2.0 * PI * u_freqs[i];
        h += u_amps[i] * sin(phase);
    }
    return h;
}

vec3 tangent(vec3 vert) {
    float dhdx = 0.0;
    for(int i = 0; i < NUMWAVES; ++i) {
        float phase = dot(u_dirs[i], vert.xz) * 2.0 * PI * u_freqs[i] + u_time * kSpeed * 2.0 * PI * u_freqs[i];
        dhdx += u_amps[i] * cos(phase) * 2.0 * PI * u_freqs[i] * u_dirs[i].x;
    }
    return normalize(vec3(1.0, dhdx, 0.0));
}

vec3 binormal(vec3 vert) {
    float dhdz = 0.0;
    for(int i = 0; i < NUMWAVES; ++i) {
        float phase = dot(u_dirs[i], vert.xz) * 2.0 * PI * u_freqs[i] + u_time * kSpeed * 2.0 * PI * u_freqs[i];
        dhdz += u_amps[i] * cos(phase) * 2.0 * PI * u_freqs[i] * u_dirs[i].y;
    }
    return normalize(vec3(0.0, dhdz, 1.0));
}

void main() {
    vec3 displacedPos = vec3(a_position.x, displacement(a_position), a_position.z);

    FragPosW = vec3(u_model * vec4(displacedPos, 1.0));
    FragNormal = mat3(transpose(inverse(u_model)))\
    * normalize(cross(binormal(a_position), tangent(a_position)));

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0);
}