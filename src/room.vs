#version 330 core
#define NUM_SPKRS 3 // Define the maximum number of speakers/sources
layout(location = 0) in vec3 a_position; // Base position of the point on a sphere (ideally unit sphere)

// Transformation matrices (Must be uniforms)
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

// Speaker/Source data (Current Loudness/Amplitude - Uniform)
uniform float u_spkrAmplitude[NUM_SPKRS]; // Current linear amplitude for each speaker/source
uniform vec3 u_spkrPos[NUM_SPKRS];        // Position of each speaker/source (Typically uniforms)

// --- Sinc Wave Simulation Parameters (Constants) ---
// These values are hardcoded. Change them requires recompiling the shader.
const float C_BASE_SPATIAL_FREQUENCY = 5.0;       // Base frequency of the sinc wave (controls ripple density)
const float C_AMPLITUDE_SPATIAL_FREQUENCY_SCALE = 10.0; // How much amplitude scales the sinc wave frequency
const float C_MAX_OVERALL_DISPLACEMENT = 0.2;     // Maximum possible displacement scale from all sources combined
const float C_SPATIAL_DECAY_RATE = 2.0;           // Controls overall decay with geodesic distance from source

// Outputs to the fragment shader
out float v_displacementMagnitude; // Absolute magnitude of the total displacement
out vec3 v_normal;                 // Displaced normal for lighting

// Helper function for the normalized sinc function
float sinc(float x) {
    // Use a small epsilon to handle the singularity at x=0
    const float PI = 3.14159265359;
    if(abs(x) < 1e-5) {
        return 1.0;
    } else {
        return sin(PI * x) / (PI * x);
    }
}

void main() {
    // Base position, assumed to be on the unit sphere
    vec3 basePosition = a_position;

    // Accumulate displacement from all audio sources
    float signedDisplacementMagnitude = 0.0;

    for(int i = 0; i < NUM_SPKRS; ++i) {
        vec3 waveOrigin = u_spkrPos[i];

        // --- Calculate Geodesic Distance (Angle) from Point to Wave Origin on the Unit Sphere ---
        float dot_product = dot(normalize(basePosition), normalize(waveOrigin));
        dot_product = clamp(dot_product, -1.0, 1.0);
        float angleFromSource = acos(dot_product); // Geodesic distance (angle in radians)

        // --- Calculate Sinc Function Input ---
        // The frequency scales with amplitude: freq = baseFreq + amplitudeScale * amplitude
        // Use the current amplitude directly as there's no interpolation
        float currentSpatialFrequency = C_BASE_SPATIAL_FREQUENCY + C_AMPLITUDE_SPATIAL_FREQUENCY_SCALE * u_spkrAmplitude[i];
        float sinc_input = currentSpatialFrequency * angleFromSource;

        // --- Calculate Sinc Wave Contribution ---
        // The amplitude of the sinc wave is scaled by the current source amplitude
        float sincWaveValue = 3 * sinc(sinc_input);

        // Apply a spatial decay based on distance from source
        // float spatialDecay = exp(-C_SPATIAL_DECAY_RATE * angleFromSource);
        float spatialDecay = 1;

        // Combine factors for this source's contribution
        // No temporal oscillation in this version. The pattern is static in space per amplitude value.
        signedDisplacementMagnitude += u_spkrAmplitude[i] * sincWaveValue * spatialDecay;
    }

    // Scale the total accumulated displacement by the maximum allowed
    signedDisplacementMagnitude *= C_MAX_OVERALL_DISPLACEMENT;

    // Calculate the final displaced position
    vec3 displacementDirection = normalize(basePosition);
    vec3 displacedPosition = basePosition + displacementDirection * signedDisplacementMagnitude;

    // Calculate the displaced normal for lighting
    v_normal = normalize(displacedPosition);

    // Pass the absolute total displacement magnitude to the fragment shader
    v_displacementMagnitude = abs(signedDisplacementMagnitude);

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);
}