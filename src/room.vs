#version 330 core
#define NUM_SPKRS 3 // Define the maximum number of speakers/sources
layout(location = 0) in vec3 a_position; // Base position of the point on a sphere

// Transformation matrices
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

// Time for animation
uniform float u_time;

// Speaker/Source data
uniform float u_spkrAmplitude[NUM_SPKRS]; // Linear amplitudes from audio processing
uniform vec3 u_spkrPos[NUM_SPKRS]; // Position of each speaker/source

// --- Wave Simulation Parameters (Made Uniforms) ---
uniform float u_waveSpeed;              // Angular speed of wave propagation
uniform float u_maxOverallDisplacement; // Max displacement scale
uniform float u_spatialDecayRate;       // Decay rate FROM wave front
uniform float u_sourceDecayRate;        // Controls how quickly amplitude decays WITH GEODESIC DISTANCE FROM SOURCE
uniform float u_oscillationFrequency;   // Temporal oscillation speed

// Outputs to the fragment shader
out float v_displacementMagnitude; // Absolute magnitude of the total displacement
out vec3 v_normal;                 // Displaced normal for lighting

void main() {
    vec3 basePosition = a_position;
    float signedDisplacementMagnitude = 0.0;

    for(int i = 0; i < NUM_SPKRS; ++i) {
        vec3 waveOrigin = u_spkrPos[i];

        // --- Calculate Geodesic Distance (Angle) from Point to Wave Origin ---
        float dot_product = dot(normalize(basePosition), normalize(waveOrigin));
        dot_product = clamp(dot_product, -1.0, 1.0);
        float angleFromSource = acos(dot_product); // Geodesic distance (angle)

        // --- Calculate Wave's Current Radial Position (Distance Traveled) ---
        float waveRadialPos = mod(u_time * u_waveSpeed, 2.0 * 3.14159);

        // --- Calculate Angular Distance of Point from the Current Wave Front ---
        float distanceToWaveFront = abs(angleFromSource - waveRadialPos);
        distanceToWaveFront = min(distanceToWaveFront, 2.0 * 3.14159 - distanceToWaveFront);

        // --- Calculate Displacement Strength Components ---

        // 1. Spatial profile: decay based on distance FROM the CURRENT wave front
        float spatialStrength = exp(-u_spatialDecayRate * distanceToWaveFront * distanceToWaveFront);

        // 2. Source Decay: amplitude decays based on GEODESIC DISTANCE FROM THE SOURCE
        // This is an alternative/addition to decay based on distance traveled.
        // Exponential decay based on the angle from the source position.
        float sourceDistanceDecay = exp(-u_sourceDecayRate * angleFromSource);

        // 3. Temporal oscillation: sinusoidal oscillation based on time and source amplitude
        float timeOscillation = u_spkrAmplitude[i] * sin(u_time * u_oscillationFrequency);

        // Combine factors
        // Using sourceDistanceDecay instead of propagationDecay (based on distance traveled)
        signedDisplacementMagnitude += sourceDistanceDecay * spatialStrength * timeOscillation;
    }

    // Scale the total accumulated displacement
    signedDisplacementMagnitude *= u_maxOverallDisplacement;

    // Calculate the final displaced position
    vec3 displacementDirection = normalize(basePosition);
    vec3 displacedPosition = basePosition + displacementDirection * signedDisplacementMagnitude;

    // Calculate the displaced normal
    v_normal = normalize(displacedPosition);

    // Pass the absolute displacement magnitude
    v_displacementMagnitude = abs(signedDisplacementMagnitude);

    // Apply transformations
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);
}