#include <vector>
#include <glm/glm.hpp>

std::vector<glm::vec3> generateFibonacciSpherePoints(int numPoints) {
    std::vector<glm::vec3> points;
    points.reserve(numPoints);

    const float phi = M_PI * (3.0 - std::sqrt(5.0)); // Golden angle in radians

    for (int i = 0; i < numPoints; ++i) {
        float y = 1.0 - (i / (float)(numPoints - 1)) * 2.0; // y goes from 1 to -1
        float radius = std::sqrt(1.0 - y * y); // Radius at y
        float theta = phi * i; // Angle grows linearly

        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);

        points.push_back(glm::vec3(x, y, z));
    }

    return points;
}