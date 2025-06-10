#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

out vec3 v_FragPosW;
out vec3 v_FragNormal;

// --- Sum of Sines Parameters ---
#define N_SINES 32
uniform float u_amplitudes[N_SINES];
uniform float u_frequencies[N_SINES];
uniform float u_phases[N_SINES];
uniform vec2 u_wavedirs[N_SINES];

void main() {
    vec3 basePos = a_position;
    float totalHeight = 0.0f;
    vec3 totalNormalInfluence = vec3(0.0f);

    float dX = 0.0f;
    float dZ = 0.0f;
    for(int i = 0; i < N_SINES; ++i) {
        float waveInputX = dot(basePos.xz, vec2(u_wavedirs[i].x, 0.0f));
        float waveInputZ = dot(basePos.xz, vec2(0.0f, u_wavedirs[i].y));
        float waveInput = dot(basePos.xz + vec2(dX, dZ), u_wavedirs[i]);

        float bf = 0.018f;
        float ampBM = 1.0f - bf * float(i);
        float freqBM = 1.0f + bf * float(i);

        float sineArg = 2.0f * 3.14159f * freqBM * u_frequencies[i] * (waveInput + u_time) + u_phases[i];

        float waveHeight = ampBM * u_amplitudes[i] * exp(sin(sineArg) - 1.0f);
        totalHeight += waveHeight;

        float expTerm = exp(sin(sineArg) - 1.0f);
        float cosTerm = cos(sineArg);
        float commonFactor = 2.0f * 3.14159f * u_frequencies[i];

        dX = ampBM * u_amplitudes[i] * expTerm * cosTerm * commonFactor * u_wavedirs[i].x;
        dZ = ampBM * u_amplitudes[i] * expTerm * cosTerm * commonFactor * u_wavedirs[i].y;

        totalNormalInfluence += vec3(dX, 0.0f, dZ);
    }

    vec3 displacedPos = vec3(basePos.x, basePos.y + totalHeight, basePos.z);
    vec3 normal_obj = normalize(vec3(-totalNormalInfluence.x, 1.0f, -totalNormalInfluence.z));

    mat3 normalMatrix = mat3(transpose(inverse(u_model)));
    v_FragNormal = normalize(normalMatrix * normal_obj);

    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0f);
    v_FragPosW = vec3(u_model * vec4(displacedPos, 1.0f));
}
