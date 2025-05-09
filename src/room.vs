#version 330 core
#define NUM_SPKRS 3
layout(location = 0) in vec3 a_position; // Base position of the point

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;
uniform float u_spkrDb[NUM_SPKRS];
uniform vec3 u_spkrPos[NUM_SPKRS];

// Output the absolute displacement magnitude to the fragment shader
out float v_displacementMagnitude;

void main() {
    // --- Wave Simulation Parameters ---
    const float waveSpeed = 1.0; // How fast the wave propagates radially (adjust as needed)
    const float maxInitialDisplacement = 0.5; // Maximum displacement amplitude at the origin
    const float spatialDecayRate = 10.0; // Controls how quickly the displacement decays with distance FROM wave center
    const float propagationDecayRate = 1.5; // Controls how quickly the overall wave amplitude decays WITH DISTANCE TRAVELED from the origin
    const float oscillationFrequency = 10.0; // Controls how fast points oscillate up/down over time

    // Base position is on the unit sphere
    vec3 basePosition = a_position;

    // Calculate displacement from each audio source
    vec3 displacedPosition = basePosition;
    float signedDisplacementMagnitude = 0.0;
    for(int i = 0; i < NUM_SPKRS; ++i) {
        vec3 waveOrigin = u_spkrPos[i];
        // --- Calculate Surface Distance from Point to Wave Origin (Top) ---
        float dot_product = dot(normalize(basePosition), normalize(waveOrigin));
        dot_product = clamp(dot_product, -1.0, 1.0);
        float angleFromOrigin = acos(dot_product); // Angle in radians (distance from top pole on surface, 0 to PI)

        // --- Calculate Wave's Current Radial Position (Distance Traveled) ---
        float waveRadialPos = mod(u_time * waveSpeed, 2.0 * 3.14159);

        // --- Calculate Distance of Point from Current Wave Radial Position ---
        float distanceToWaveCenter = abs(angleFromOrigin - waveRadialPos);
        distanceToWaveCenter = min(distanceToWaveCenter, 2.0 * 3.14159 - distanceToWaveCenter);

        // --- Calculate Displacement Strength (Spatial and Temporal) ---

        // Spatial profile: displacement strength based on distance from the wave's current center
        float spatialStrength = exp(-spatialDecayRate * distanceToWaveCenter * distanceToWaveCenter);

        // Propagation decay: overall wave amplitude decays based on distance TRAVELLED from the origin (top)
        float propagationDecay = exp(-propagationDecayRate * waveRadialPos);

        // Temporal oscillation: A sinusoidal function of time
        float timeOscillation = u_spkrDb[i] * sin(u_time * oscillationFrequency);

        // Combine factors to get the signed displacement magnitude
        signedDisplacementMagnitude = signedDisplacementMagnitude + maxInitialDisplacement * propagationDecay * spatialStrength * timeOscillation;

        // Calculate the final displaced position
        // Displacement direction is outwards along the normal (normalized base position)
        vec3 displacementDirection = normalize(basePosition);
        displacedPosition = displacedPosition + displacementDirection * signedDisplacementMagnitude;
    }

    // Pass the absolute displacement magnitude to the fragment shader
    v_displacementMagnitude = abs(signedDisplacementMagnitude);

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);
}