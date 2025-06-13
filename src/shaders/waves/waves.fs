#version 330 core
in vec3 FragPosW;

out vec4 FragColor;

void main() {
    vec3 BaseColor = vec3(0.22, 0.27, 0.51);

    FragColor = vec4(BaseColor, 1.0);
}