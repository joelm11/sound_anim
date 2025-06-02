#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 FragPosW;
out vec3 FragNormal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

const float kFreq = 1.0;

void main() {
    vec3 basePos = a_position;
    // Calculate vertex displacement
    float height = sin(2 * 3.14159 * kFreq * u_time + basePos.z);

    // Calculate surface normal
    float dZ = cos(2 * 3.14159 * kFreq * u_time + basePos.z);
    float dX = 0.0;
    vec3 tanX = vec3(1.0, dX, 0.0);
    vec3 tanZ = vec3(0.0, dZ, 1.0);
    vec3 normal_obj = normalize(cross(tanZ, tanX));
    mat3 normalMatrix = mat3(transpose(inverse(u_model)));
    FragNormal = normalize(normalMatrix * normal_obj); // Normal in worldspace

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(basePos.x, basePos.y + height, basePos.z, 1.0);
    FragPosW = vec3(u_model * vec4(a_position, 1.0)); // Position in worldspace
}