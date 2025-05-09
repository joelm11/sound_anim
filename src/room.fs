#version 330 core
out vec4 FragColor;

void main()
{
    // Use gl_PointCoord to make the points round
    float distToCenter = distance(gl_PointCoord, vec2(0.5, 0.5));
    if (distToCenter > 0.5) {
        discard; // Discard fragments outside the circle
    }

    // Set the color (e.g., green as before)
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}