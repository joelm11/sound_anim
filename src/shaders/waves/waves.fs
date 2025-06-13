#version 330 core
in vec3 FragPosW;
in vec3 FragNormal;

out vec4 FragColor;

uniform vec3 u_lightpos;
uniform vec3 u_camerapos;

uniform samplerCube u_skyboxTexture;

const float ambientStrength = 0.1;
const float diffuseStrength = 1.0;
const vec3 lightColor = vec3(0.46, 0.67, 0.77);
const vec3 ambientColor = vec3(1.0);

uniform float u_mieCoefficient; // New uniform for Mie scattering coefficient
uniform float u_horizonBlurStrength; // Add a uniform for horizon blur strength
uniform float u_horizonHeight; // Add a uniform for horizon height

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

    // // Calculate reflection vector
    // vec3 reflectionVector = reflect(normalize(u_camerapos - FragPosW), norm);
    // // Sample the skybox texture
    // vec3 skyboxColor = texture(u_skyboxTexture, reflectionVector).rgb;

    // // Combine the wave color with the skybox color
    // float reflectionStrength = 0.3; // Adjust this value to control the intensity of the reflection
    // vec3 reflectedColor = mix(finalColor, skyboxColor, reflectionStrength);

    // Calculate distance from camera to fragment
    float distance = length(u_camerapos - FragPosW);

    // Apply Mie scattering (atmospheric scattering)
    float mieFactor = exp(-u_mieCoefficient * distance);
    vec3 finalColorWithMie = mix(finalColor, finalColor, mieFactor);

    // Calculate horizon blur factor
    float horizonDistance = abs(FragPosW.y - u_horizonHeight);
    float blurFactor = clamp(horizonDistance * u_horizonBlurStrength, 0.0, 1.0);

    // Mix the final color with the skybox color based on the blur factor
    vec3 blurredColor = mix(finalColorWithMie, finalColor, blurFactor);

    FragColor = vec4(blurredColor, 1.0);
}