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
#define N_SINES 3 // Number of sines

// Amplitude for each sine wave
uniform float u_amplitudes[N_SINES];
// Frequency for each sine wave
uniform float u_frequencies[N_SINES];
// Phase offset for each sine wave
uniform float u_phases[N_SINES];
// Direction of propagation (1.0 for X, 0.0 for Z; or 0.0 for X, 1.0 for Z)
// Or a combination for diagonal waves, e.g., vec2(0.707, 0.707)
uniform vec2 u_wavedirs[N_SINES]; // X and Z components for propagation direction

void main() {
    vec3 basePos = a_position;
    float totalHeight = 0.0;
    vec3 totalNormalInfluence = vec3(0.0); // For accumulating normal contributions

    // Calculate vertex displacement and normal influence for each sine wave
    for(int i = 0; i < N_SINES; ++i) {
        // Calculate the dot product with the position for wave propagation direction
        float waveInputX = dot(basePos.xz, vec2(u_wavedirs[i].x, 0.0));
        float waveInputZ = dot(basePos.xz, vec2(0.0, u_wavedirs[i].y));
        float waveInput = waveInputX + waveInputZ; // Sum for combined X and Z propagation

        // Individual wave height
        float waveHeight = u_amplitudes[i] * sin(2.0 * 3.14159 * u_frequencies[i] * (waveInput + u_time) + u_phases[i]);
        totalHeight += waveHeight;

        // Individual normal contribution (derivative of sine)
        // We're calculating the derivative with respect to basePos.x and basePos.z
        // d/dx (A * sin(2*pi*f*(x*dir.x + z*dir.y + t) + p))
        //   = A * cos(...) * 2*pi*f * dir.x
        // d/dz (A * sin(2*pi*f*(x*dir.x + z*dir.y + t) + p))
        //   = A * cos(...) * 2*pi*f * dir.y
        float cosTerm = cos(2.0 * 3.14159 * u_frequencies[i] * (waveInput + u_time) + u_phases[i]);

        float dX = u_amplitudes[i] * cosTerm * 2.0 * 3.14159 * u_frequencies[i] * u_wavedirs[i].x;
        float dZ = u_amplitudes[i] * cosTerm * 2.0 * 3.14159 * u_frequencies[i] * u_wavedirs[i].y;

        // Accumulate these derivatives. These are the components of the surface gradient.
        // The normal will be proportional to (-dX, 1, -dZ) before normalization.
        totalNormalInfluence += vec3(dX, 0.0, dZ);
    }

    // Apply vertex displacement
    vec3 displacedPos = vec3(basePos.x, basePos.y + totalHeight, basePos.z);

    // Calculate surface normal for the sum of sines
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