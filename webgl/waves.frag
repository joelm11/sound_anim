#version 300 es
precision highp float;

in vec3 v_FragPosW;
in vec3 v_FragNormal;

out vec4 FragColor;

uniform vec3 u_lightpos;
uniform vec3 u_camerapos;

uniform samplerCube u_skyboxTexture;

const float ambientStrength = 0.1;
const float diffuseStrength = 1.0;
const vec3 lightColor = vec3(0.46, 0.67, 0.77);
const vec3 ambientColor = vec3(0.69, 0.58, 0.4);

uniform float u_mieCoefficient;
uniform float u_horizonBlurStrength;
uniform float u_horizonHeight;

void main() {
    vec3 BaseColor = vec3(0.22, 0.27, 0.51);

    vec3 ambient = ambientStrength * ambientColor;
    vec3 norm = normalize(v_FragNormal);
    vec3 lightDir = normalize(u_lightpos - v_FragPosW);
    float diffIntensity = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diffIntensity * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_camerapos - v_FragPosW);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 finalColor = (ambient + diffuse + specular) * BaseColor;

    vec3 reflectionVector = reflect(normalize(u_camerapos - v_FragPosW), norm);
    vec3 skyboxColor = texture(u_skyboxTexture, reflectionVector).rgb;

    float reflectionStrength = 0.3;
    vec3 reflectedColor = mix(finalColor, skyboxColor, reflectionStrength);

    float distance = length(u_camerapos - v_FragPosW);

    float mieFactor = exp(-u_mieCoefficient * distance);
    vec3 finalColorWithMie = mix(skyboxColor, reflectedColor, mieFactor);

    float horizonDistance = abs(v_FragPosW.y - u_horizonHeight);
    float blurFactor = clamp(horizonDistance * u_horizonBlurStrength, 0.0, 1.0);

    vec3 blurredColor = mix(finalColorWithMie, skyboxColor, blurFactor);

    FragColor = vec4(blurredColor, 1.0);
}
