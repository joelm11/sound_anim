#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal; // If you have a_normal, otherwise remove

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

out vec3 FragPosW;
out vec3 FragNormal;

// --- Sum of Sines Parameters ---
#define N_SINES 32
uniform float u_amplitudes[N_SINES];
uniform float u_frequencies[N_SINES];
uniform float u_phases[N_SINES];
uniform vec2 u_wavedirs[N_SINES];

void main() {
    vec3 basePos = a_position;
    float totalHeight = 0.0;
    vec3 totalNormalInfluence = vec3(0.0); // For accumulating normal contributions

    // Calculate vertex displacement and normal influence for each wave
    float dX = 0.0;
    float dZ = 0.0;
    for(int i = 0; i < N_SINES; ++i) {
        // Calculate the dot product with the position for wave propagation direction
        float waveInputX = dot(basePos.xz, vec2(u_wavedirs[i].x, 0.0));
        float waveInputZ = dot(basePos.xz, vec2(0.0, u_wavedirs[i].y));
        float waveInput = dot(basePos.xz + vec2(dX, dZ), u_wavedirs[i]);

        // Brownian(esque)?
        float bf = 0.018;
        float ampBM = 1.0 - bf * i;
        float freqBM = 1.0 + bf * i;

        // Calculate the argument for the sine function within the exponential
        float sineArg = 2.0 * 3.14159 * freqBM * u_frequencies[i] * (waveInput + u_time) + u_phases[i];

        // Individual wave height using e^(sin(x) - 1)
        float waveHeight = ampBM * u_amplitudes[i] * exp(sin(sineArg) - 1.0);
        totalHeight += waveHeight;

        // Individual normal contribution (derivative of e^(sin(x) - 1))
        // The derivative of A * e^(sin(G(x,z,t)) - 1) with respect to x (or z)
        // is A * e^(sin(G) - 1) * cos(G) * dG/dx (or dG/dz)
        // where G = 2*pi*f*(x*dir.x + z*dir.y + t) + p
        // dG/dx = 2*pi*f * dir.x
        // dG/dz = 2*pi*f * dir.y
        float expTerm = exp(sin(sineArg) - 1.0);
        float cosTerm = cos(sineArg);
        float commonFactor = 2.0 * 3.14159 * u_frequencies[i];

        dX = ampBM * u_amplitudes[i] * expTerm * cosTerm * commonFactor * u_wavedirs[i].x;
        dZ = ampBM * u_amplitudes[i] * expTerm * cosTerm * commonFactor * u_wavedirs[i].y;

        // Accumulate these derivatives. These are the components of the surface gradient.
        totalNormalInfluence += vec3(dX, 0.0, dZ);
    }

    // Apply vertex displacement
    vec3 displacedPos = vec3(basePos.x, basePos.y + totalHeight, basePos.z);

    // Calculate surface normal for the sum of waves
    // The surface normal is perpendicular to the gradient vector (dX, dY, dZ)
    // where dY is 1 because our 'y' coordinate is the height, and we are computing
    // the normal to the displaced surface.
    // The gradient vector in 3D would be (df/dx, df/dy, df/dz). Here, f is our height function.
    // The normal vector is normalized(-df/dx, 1, -df/dz) for a height map where y = f(x,z)
    vec3 normal_obj = normalize(vec3(-totalNormalInfluence.x, 1.0, -totalNormalInfluence.z));

    mat3 normalMatrix = mat3(transpose(inverse(u_model)));
    FragNormal = normalize(normalMatrix * normal_obj); // Normal in worldspace

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPos, 1.0);
    FragPosW = vec3(u_model * vec4(a_position, 1.0)); // Position in worldspace
}