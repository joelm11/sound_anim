#include <glm/glm.hpp>
#include <vector>

inline std::vector<glm::vec3> generateFibonacciSpherePoints(int numPoints) {
  std::vector<glm::vec3> points;
  points.reserve(numPoints);

  const float phi = M_PI * (3.0 - std::sqrt(5.0)); // Golden angle in radians

  for (int i = 0; i < numPoints; ++i) {
    float y = 1.0 - (i / (float)(numPoints - 1)) * 2.0; // y goes from 1 to -1
    float radius = std::sqrt(1.0 - y * y);              // Radius at y
    float theta = phi * i;                              // Angle grows linearly

    float x = radius * std::cos(theta);
    float z = radius * std::sin(theta);

    points.push_back(glm::vec3(x, y, z));
  }

  return points;
}

inline glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
  // Radius in the XZ plane
  float xz_radius = r * std::sin(theta);

  // Cartesian coordinates
  float x = xz_radius * std::cos(phi);
  float y =
      r * std::cos(theta); // Y is based directly on the angle from the Y-axis
  float z = xz_radius * std::sin(phi);

  return glm::vec3(x, y, z);
}
