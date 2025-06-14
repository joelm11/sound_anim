#version 330 core
in vec3 FragPosW;
in vec3 FragNormal;

uniform vec3 u_lightPos;

out vec4 FragColor;

vec3 phong_lighting(vec3 baseColor) {
    const float kAmbientStrength = 0.1;
    const vec3 kLightColor = vec3(1., 1., 1.);

    vec3 ambient = kAmbientStrength * kLightColor;

    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(u_lightPos - FragPosW);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * kLightColor;

    vec3 result = (ambient + diffuse) * baseColor;
    return result;
}

void main() {
    vec3 BaseColor = vec3(0.22, 0.27, 0.51);

    FragColor = vec4(phong_lighting(BaseColor), 1.0);
}