#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

#define NUMWAVES 32
uniform float u_amps[NUMWAVES];
uniform float u_freqs[NUMWAVES];
uniform float u_speeds[NUMWAVES];
uniform vec2 u_dirs[NUMWAVES];

out vec3 FragPosW;
out vec3 FragNormal;

#define PI 3.14159
const float kSpeed = 0.2;
const float kK = 2.5;
const float kNumWaves = NUMWAVES;

float phase_const(vec3 vert, vec2 dir, float freq, float speed) {
    return dot(dir, vert.xz) * 2.0 * PI * freq + u_time * speed * 2.0 * PI * freq;
}

float displacement(vec3 vert) {
    float h = 0.0;
    for(int i = 0; i < kNumWaves; ++i) {
        float phase = phase_const(vert, u_dirs[i], u_freqs[i], u_speeds[i]);
        h += 2 * u_amps[i] * pow((sin(phase) + 1) / 2, kK);
    }
    return h / kNumWaves;
}

vec3 tangent(vec3 vert) {
    float dhdx = 0.0;
    for(int i = 0; i < kNumWaves; ++i) {
        float phase = phase_const(vert, u_dirs[i], u_freqs[i], u_speeds[i]);
        dhdx += kK * u_dirs[i].x * 2.0 * PI * u_freqs[i] * u_amps[i] * pow((sin(phase) + 1) / 2, kK - 1) * cos(phase);
    }
    return normalize(vec3(1.0, dhdx / kNumWaves, 0.0));
}

vec3 binormal(vec3 vert) {
    float dhdz = 0.0;
    for(int i = 0; i < kNumWaves; ++i) {
        float phase = phase_const(vert, u_dirs[i], u_freqs[i], u_speeds[i]);
        dhdz += kK * u_dirs[i].y * 2.0 * PI * u_freqs[i] * u_amps[i] * pow((sin(phase) + 1) / 2, kK - 1) * cos(phase);
    }
    return normalize(vec3(0.0, dhdz / kNumWaves, 1.0));
}

void main() {
    vec3 displacedPos = vec3(a_position.x, displacement(a_position), a_position.z);

    FragPosW = vec3(u_model * vec4(displacedPos, 1.0));
    FragNormal = mat3(transpose(inverse(u_model))) \
    * normalize(cross(binormal(a_position), tangent(a_position)));

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0);
}