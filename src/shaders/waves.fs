#version 330 core
in vec3 FragPosW;
in vec3 FragNormal;

out vec4 FragColor;

uniform vec3 u_lightpos;
uniform vec3 u_camerapos;

const float ambientStrength = 0.1;
const float diffuseStrength = 1.0;
const vec3 lightColor = vec3(1.0);
const vec3 ambientColor = vec3(0.69, 0.58, 0.4);

void main() {
    vec3 BaseColor = vec3(0.22, 0.27, 0.51);

    // Calculate lighting components
    // Ambient
    vec3 ambient = ambientStrength * ambientColor;
    // Diffuse
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(u_lightpos - FragPosW);
    float diffIntensity = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diffIntensity * lightColor;
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_camerapos - FragPosW);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 finalColor = (ambient + diffuse + specular) * BaseColor;
    FragColor = vec4(finalColor, 1.0);
}