#version 330 core

out vec4 FragColor;

in vec3 localPos;

uniform samplerCube u_skyboxTexture;

void main() {
    // Use the localPos as texture coordinates
    FragColor = texture(u_skyboxTexture, localPos);
}