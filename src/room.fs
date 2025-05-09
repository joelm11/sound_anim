#version 330 core
out vec4 FragColor;

// Receive the interpolated absolute displacement magnitude from the vertex shader
in float v_displacementMagnitude;

void main() {
    // Use gl_PointCoord to make the points round
    float distToCenter = distance(gl_PointCoord, vec2(0.5, 0.5));
    if(distToCenter > 0.5) {
        discard; // Discard fragments outside the circle
    }

    // --- Color based on Displacement ---

    // Define the base color (e.g., green)
    vec3 baseColor = vec3(0.0, 1.0, 0.0);

    // Define a color to blend towards at maximum displacement (e.g., yellow)
    vec3 peakColor = vec3(1.0, 0.0, 0.0);

    // Estimate the maximum possible displacement magnitude
    // This is tricky because it depends on the wave's current state and location.
    // A safe upper bound is maxInitialDisplacement * 1.0 (when propagationDecay and spatialStrength are max)
    // You might need to experiment to find a good value for normalization.
    float maxExpectedDisplacement = .1; // Simple estimate

    // Normalize the current displacement magnitude to a 0-1 range
    // Clamp to ensure it stays within 0-1 in case estimation is off
    float colorMixFactor = clamp(v_displacementMagnitude / maxExpectedDisplacement, 0.0, 1.0);

    // Interpolate between the base color and the peak color based on the normalized displacement
    vec3 finalColor = mix(baseColor, peakColor, colorMixFactor);

    // Set the final fragment color
    FragColor = vec4(finalColor, 1.0); // Use 1.0 alpha for solid points
}