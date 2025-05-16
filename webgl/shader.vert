#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;
uniform vec3 u_spkrPos[2];
uniform float u_spkrDb[2];
uniform float u_waveSpeed;
uniform float u_maxOverallDisplacement;
uniform float u_spatialDecayRate;
uniform float u_sourceDecayRate;
uniform float u_oscillationFrequency;

out float v_wave;
out vec3 v_normal;
out vec3 v_pos;

void main() {
    vec3 pos = aPos;
    float wave = 0.0f;
    for(int i = 0; i < 2; ++i) {
        float dist = distance(pos, u_spkrPos[i]);
        float t = u_time - dist / u_waveSpeed;
        float amp = u_spkrDb[i] * u_maxOverallDisplacement * exp(-u_spatialDecayRate * dist) * exp(-u_sourceDecayRate * max(0.0f, t));
        wave += amp * sin(u_oscillationFrequency * t);
    }
    v_wave = wave;
    v_pos = pos + wave * normalize(pos);
    v_normal = normalize(pos);
    gl_Position = u_projection * u_view * u_model * vec4(v_pos, 1.0f);
    gl_PointSize = 7.0f;
}
