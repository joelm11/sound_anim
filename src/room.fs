#version 330 core

// Input varying variables from the vertex shader
in float v_displacementMagnitude; // Absolute magnitude of the total displacement at this vertex
in vec3 v_normal;                 // The recalculated normal vector of the displaced vertex (in a consistent space, e.g., World or View)

// Output fragment color
out vec4 FragColor;

// --- Lighting Uniforms ---
// Assuming a simple directional light model for this example
uniform vec3 u_lightDir;   // Direction *to* the light source (e.g., in the same space as v_normal)
uniform vec3 u_lightColor; // Color/intensity of the light
uniform vec3 u_ambientColor; // Ambient light color

// --- Material/Coloring Uniforms ---
uniform vec3 u_baseColor;       // Color for areas with low or no displacement
uniform vec3 u_wavePeakColor;   // Color for areas with high displacement (wave crests/troughs)
uniform float u_waveColorScale; // Scales the v_displacementMagnitude to map it to the [0, 1] range for color interpolation
uniform float u_waveColorOffset; // Optional offset for the displacement mapping

void main() {
    // --- Basic Lighting ---
    // Ensure the normal is normalized (should be from VS, but good practice)
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightDir);

    // Ambient component
    vec3 ambient = u_ambientColor * u_baseColor; // Apply ambient light to the base color

    // Diffuse component (Lambertian reflectance)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_lightColor * u_baseColor * diff; // Apply diffuse light to the base color

    // Combine lighting for the base surface
    vec3 baseLighting = ambient + diffuse;

    // --- Wave Coloring based on Displacement ---

    // Map displacement magnitude to a value between 0 and 1
    // Use scale and offset, then clamp to ensure it's in the [0, 1] range
    float displacementFactor = clamp(v_displacementMagnitude * u_waveColorScale + u_waveColorOffset, 0.0, 1.0);

    // Interpolate between the base color and the wave peak color
    vec3 waveColor = mix(u_baseColor, u_wavePeakColor, displacementFactor);

    // --- Combine Lighting and Wave Coloring ---

    // A simple way: use the wave color as the surface color and apply the base lighting to it.
    // This makes the colors driven by displacement, and the shading driven by normals and light.
    vec3 finalColor = waveColor * baseLighting;

    // You could also blend the lighting results:
    // vec3 waveLighting = u_lightColor * u_wavePeakColor * diff; // Simplified diffuse for wave peaks
    // vec3 finalColor = mix(baseLighting, ambient + waveLighting, displacementFactor); // Mix base lighting with wave lighting

    FragColor = vec4(finalColor, 1.0); // Output the final color
}