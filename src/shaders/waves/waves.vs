#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

out vec3 FragPosW;
out vec3 FragNormal;

#define PI 3.14159

const float kAmp = 0.5;
const float kSpeed = 1.0;
const float kFreq = 1.0;
const vec2 kDir = vec2(1.0, 0.0);

float displacement(vec3 vert) {
    float phase = dot(kDir, vert.xz) * 2.0 * PI * kFreq + u_time * kSpeed * 2.0 * PI * kFreq;
    float h = kAmp * sin(phase);
    return h;
}

vec3 tangent(vec3 vert) {
    float phase = dot(kDir, vert.xz) * 2.0 * PI * kFreq + u_time * kSpeed * 2.0 * PI * kFreq;
    float dhdx = kAmp * cos(phase) * 2.0 * PI * kFreq * kDir.x;
    return normalize(vec3(1.0, dhdx, 0.0));
}

vec3 binormal(vec3 vert) {
    float phase = dot(kDir, vert.xz) * 2.0 * PI * kFreq + u_time * kSpeed * 2.0 * PI * kFreq;
    float dhdz = kAmp * cos(phase) * 2.0 * PI * kFreq * kDir.y;
    return normalize(vec3(0.0, dhdz, 1.0));
}

void main() {
    vec3 displacedPos = vec3(a_position.x, displacement(a_position), a_position.z);

    FragPosW = vec3(u_model * vec4(displacedPos, 1.0));
    FragNormal = normalize(cross(binormal(a_position), tangent(a_position)));

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0);
}