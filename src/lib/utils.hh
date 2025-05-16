#pragma once
#include "glm/ext/vector_float3.hpp"
#include <vector>

inline std::vector<glm::vec3> unitSquareXPlanePoints(int num_points) {
  // Calculate N, the side length of the grid (e.g., if num_points=9, N=3)
  double N_double = std::sqrt(static_cast<double>(num_points));
  int N = static_cast<int>(std::round(N_double));

  std::vector<glm::vec3> points;
  points.reserve(num_points);

  // Calculate the step size between points
  // The range is from -1.0 to 1.0, which is a total span of 2.0 units.
  // For N points, there are (N-1) intervals.
  float step_size = 2.0f / static_cast<float>(N - 1);

  // Generate points using nested loops for rows and columns
  for (int row = 0; row < N; ++row) {
    for (int col = 0; col < N; ++col) {
      // Calculate x and y coordinates
      // Start at -1.0 and add (index * step_size)
      float x = -1.0f + static_cast<float>(col) * step_size;
      float z = -1.0f + static_cast<float>(row) * step_size;

      points.emplace_back(x, 0.0f, z);
    }
  }

  return points;
}