// Vertex Shader (room.vs)
#version 330 core
layout(location = 0) in vec3 a_position; // Base position of the point

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time; // Time uniform for animation

// Point size needs to be set in the vertex shader in OpenGL 3.3+ Core
out float gl_PointSize;

void main() {
    // Base position is on the unit sphere
    vec3 basePosition = a_position;

    // --- Wave Simulation Parameters ---
    vec3 waveOrigin = vec3(0.0, 1.0, 0.0); // Top of the sphere (assuming Y is up)
    float waveSpeed = 1.0; // How fast the wave propagates radially (adjust as needed)
    float maxInitialDisplacement = 0.5; // Maximum displacement amplitude at the origin
    float spatialDecayRate = 20.0; // Controls how quickly the displacement decays with distance FROM wave center
    float propagationDecayRate = 1.5; // Controls how quickly the overall wave amplitude decays WITH DISTANCE TRAVELED from the origin
    float oscillationFrequency = 5.0; // Controls how fast points oscillate up/down over time

    // --- Calculate Surface Distance from Point to Wave Origin (Top) ---
    // Distance on a unit sphere's surface is the angle between the normalized vectors
    float dot_product = dot(normalize(basePosition), normalize(waveOrigin));
    dot_product = clamp(dot_product, -1.0, 1.0); // Clamp to avoid precision issues
    float angleFromOrigin = acos(dot_product); // Angle in radians (distance from top pole on surface, 0 to PI)

    // --- Calculate Wave's Current Radial Position (Distance Traveled) ---
    // The wave front propagates outwards from the origin (from angle 0 towards PI)
    // We'll have it reset after reaching the bottom (PI radians) for simplicity
    float waveRadialPos = mod(u_time * waveSpeed, 2.0 * 3.14159); // Propagate across the full sphere circumference angle

    // --- Calculate Distance of Point from Current Wave Radial Position ---
    // How far is the point's angleFromOrigin from the current wave's center position?
    float distanceToWaveCenter = abs(angleFromOrigin - waveRadialPos);

     // Account for wrap-around if using the full 2*PI modulation
    distanceToWaveCenter = min(distanceToWaveCenter, 2.0 * 3.14159 - distanceToWaveCenter);

    // --- Calculate Displacement Strength (Spatial and Temporal) ---

    // Spatial profile: displacement strength based on distance from the wave's current center
    // Strongest at the wave center (distanceToWaveCenter = 0), decays away
    float spatialStrength = exp(-spatialDecayRate * distanceToWaveCenter * distanceToWaveCenter);

    // Propagation decay: overall wave amplitude decays based on distance TRAVELLED from the origin (top)
    // The wave is strongest at the origin (waveRadialPos = 0) and gets weaker as waveRadialPos increases
    float propagationDecay = exp(-propagationDecayRate * waveRadialPos);

    // Temporal oscillation: A sinusoidal function of time
    float timeOscillation = sin(u_time * oscillationFrequency);

    // Combine factors: max amplitude * decay with distance traveled * decay from wave center * time oscillation
    float displacementMagnitude = maxInitialDisplacement * propagationDecay * spatialStrength * timeOscillation;

    // Direction of displacement is outwards along the normal (normalized base position)
    vec3 displacementDirection = normalize(basePosition);

    // Calculate the final displaced position
    vec3 displacedPosition = basePosition + displacementDirection * displacementMagnitude;

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);

    // Set the point size (adjust as needed)
    gl_PointSize = 5.0; // Keep the point size for visibility
}