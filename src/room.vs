// Vertex Shader (room.vs)
#version 330 core
layout(location = 0) in vec3 a_position; // Base position of the point

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time; // Time uniform for animation

void main() {
    // Base position is on the unit sphere
    vec3 basePosition = a_position;

    // --- Wave Simulation Parameters ---
    vec3 waveOrigin = vec3(0.0, 1.0, 0.0); // Top of the sphere (assuming Y is up)
    float waveSpeed = 1.0; // How fast the wave propagates radially (adjust as needed)
    float maxDisplacement = 0.5; // Maximum displacement magnitude
    float decayRate = 20.0; // Controls how quickly the displacement decays with distance from wave center
    float oscillationFrequency = 5.0; // Controls how fast points oscillate up/down over time

    // --- Calculate Surface Distance from Point to Wave Origin (Top) ---
    // Distance on a unit sphere's surface is the angle between the normalized vectors
    // Use the dot product and acos to find the angle in radians
    // Clamp dot product to avoid issues with floating point precision near 1 or -1
    float dot_product = dot(normalize(basePosition), normalize(waveOrigin));
    dot_product = clamp(dot_product, -1.0, 1.0);
    float angleFromOrigin = acos(dot_product); // Angle in radians (distance from top pole on surface)

    // --- Calculate Wave's Current Radial Position ---
    // The wave front propagates outwards from the origin over time (from angle 0 towards PI)
    // Use mod to make the wave wrap around the sphere vertically (e.g., resurface from bottom)
    float waveRadialPos = mod(u_time * waveSpeed, 2.0 * 3.14159); // Wave wraps around the full circle

    // Consider a simpler wrap that resets after reaching the bottom (PI)
    // float waveRadialPos = mod(u_time * waveSpeed, 3.14159); // Wave travels from top to bottom

    // --- Calculate Distance of Point from Current Wave Radial Position ---
    // How far is the point's angleFromOrigin from the current wave's radial position?
    float distanceToWave = abs(angleFromOrigin - waveRadialPos);

    // Account for wrap-around if using the full 2*PI modulation
    distanceToWave = min(distanceToWave, 2.0 * 3.14159 - distanceToWave);

    // --- Calculate Displacement Strength (Spatial and Temporal) ---

    // Spatial decay: displacement strength based on distance from the wave's core
    // Use an exponential decay (Gaussian-like shape)
    float spatialStrength = exp(-decayRate * distanceToWave * distanceToWave); // Exponential of negative distance squared for sharper falloff

    // Temporal oscillation: A sinusoidal function of time
    // This makes the points oscillate up and down as the wave affects them
    float timeOscillation = sin(u_time * oscillationFrequency);

    // Combine spatial decay and temporal oscillation
    float displacementMagnitude = maxDisplacement * spatialStrength * timeOscillation;

    // Direction of displacement is outwards along the normal (normalized base position)
    vec3 displacementDirection = normalize(basePosition);

    // Calculate the final displaced position
    vec3 displacedPosition = basePosition + displacementDirection * displacementMagnitude;

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);
}