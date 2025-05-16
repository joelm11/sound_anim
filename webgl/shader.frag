#version 300 es
precision highp float;

in float v_wave;
in vec3 v_normal;
in vec3 v_pos;

uniform vec3 u_lightDir;
uniform vec3 u_lightColor;
uniform vec3 u_ambientColor;
uniform vec3 u_baseColor;
uniform vec3 u_wavePeakColor;
uniform float u_waveColorScale;

out vec4 FragColor;

void main() {
    float light = max(dot(normalize(v_normal), normalize(-u_lightDir)), 0.0);
    vec3 color = mix(u_baseColor, u_wavePeakColor, clamp(abs(v_wave) * u_waveColorScale, 0.0, 1.0));
    color = color * (u_ambientColor + u_lightColor * light);
    FragColor = vec4(color, 1.0);
}
