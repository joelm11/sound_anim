#version 330 core
layout(location = 0) in vec3 a_position; // Base position of the point

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

void main() {
    // Base position is on the unit sphere
    vec3 basePosition = a_position;

    // --- Wave Simulation Parameters ---
    vec3 waveOrigin = vec3(0.0, 1.0, 0.0); // Top of the sphere (assuming Y is up)
    float waveSpeed = 0.5; // How fast the wave propagates across the surface (adjust as needed)
    float maxDisplacement = 0.5; // Maximum displacement magnitude
    float decayRate = 20.0; // Controls how quickly the displacement decays with distance
    float waveWidth = 0.4; // Controls the width of the wave's influence zone
    float waveAmp = 0.5;

    // --- Calculate Surface Distance from Point to Wave Origin ---
    // Distance on a unit sphere's surface is the angle between the normalized vectors
    // Use the dot product and acos to find the angle in radians
    float angleFromOrigin = waveAmp * acos(dot(normalize(basePosition), normalize(waveOrigin)));

    // --- Calculate Wave Position and Influence ---
    // The wave front propagates outwards from the origin over time
    float currentWaveDistance = mod(u_time * waveSpeed, 2.0 * 3.14159); // Wave wraps around the sphere horizontally based on angle

    // Calculate the distance of the point from the current wave front
    // We'll consider the wave propagating radially outwards from the top (angle 0)
    // The distance on the surface is the angle itself
    float distanceToWaveFront = abs(angleFromOrigin - currentWaveDistance);

    // Account for wrapping around the sphere for shortest distance
    distanceToWaveFront = min(distanceToWaveFront, 2.0 * 3.14159 - distanceToWaveFront);

    // --- Calculate Displacement ---
    float displacement = 0.0;

    // Apply displacement only within a certain range around the wave front
    // A simple approach is a smooth step or exponential decay within a window
    if(distanceToWaveFront < waveWidth) {
        //  // Calculate decay based on distance from the nearest edge of the wave front
        // float decayDistance = distanceToWaveFront; // Decay based on distance to the wave front's center line

        // // Simple exponential decay: displacement is highest at the wavefront (decayDistance=0) and drops off
        // displacement = maxDisplacement * exp(-decayRate * decayDistance);

        //  // You could also model a peak that travels, and decay based on distance from the peak:
        // float peakDistance = u_time * waveSpeed; // Peak is at this distance from origin
        // float distanceToPeak = abs(angleFromOrigin - peakDistance);
        // distanceToPeak = min(distanceToPeak, 2.0 * 3.14159 - distanceToPeak);
        // displacement = maxDisplacement * exp(-decayRate * distanceToPeak);

        //  // Or, create a pulse-like effect that travels:
        // float pulsePosition = mod(u_time * waveSpeed, 2.0 * 3.14159 + waveWidth * 2.0) - waveWidth;
        // float relativePosition = angleFromOrigin - pulsePosition;
        // if(relativePosition > -waveWidth && relativePosition < waveWidth) {
        //     // Simple linear falloff within the pulse width
        //     float strength = 1.0 - abs(relativePosition) / waveWidth;
        //     displacement = maxDisplacement * strength;
        //     // Or exponential/smoothstep falloff
        // }

    }

    // Direction of displacement is outwards along the normal (normalized base position)
    vec3 displacementDirection = normalize(basePosition);

    // Calculate the final displaced position
    vec3 displacedPosition = basePosition + displacementDirection * displacement;

    // Apply Model, View, and Projection matrices
    gl_Position = u_projection * u_view * u_model * vec4(displacedPosition, 1.0);
}