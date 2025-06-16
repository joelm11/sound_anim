#version 330 core
in vec3 FragPosW;
in vec3 FragNormal;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_camerapos;
uniform samplerCube u_skyboxTexture;

out vec4 FragColor;

vec3 phong_lighting(vec3 baseColor) {
    const float kAmbientStrength = 0.1;
    float kSpecularStrength = 0.5;
    const vec3 kLightColor = vec3(1., 1., 1.);

    // Ambient calculation
    vec3 ambient = kAmbientStrength * kLightColor;

    // Diffuse calculation
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(u_lightPos - FragPosW);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * kLightColor;

    // Specular calculation
    vec3 viewDir = normalize(u_camerapos - FragPosW);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = kSpecularStrength * spec * kLightColor;

    vec3 result = (ambient + diffuse + specular) * baseColor;
    return result;
}

void main() {
    vec3 BaseColor = vec3(58. / 255, 73. / 255, 101. / 255);
    vec3 viewDir = normalize(u_viewPos - FragPosW);
    vec3 norm = normalize(FragNormal);
    vec3 reflectDir = reflect(-viewDir, norm);
    vec3 reflectedColor = texture(u_skyboxTexture, reflectDir).rgb;
    vec3 waterColor = phong_lighting(BaseColor);
    vec3 finalColor = mix(waterColor, reflectedColor, 0.15);
    FragColor = vec4(finalColor, 1.0);
}