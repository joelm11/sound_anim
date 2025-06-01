#include "mesh.hh"

// Generates points for a plane of size num_quads x num_quads
PlaneMeshData GenerateSquarePlane(const int num_quads) {
  const float kStepSize = 2.0f / static_cast<float>(num_quads);

  // Generate points
  std::vector<Vertex> points;
  for (int i = 0; i <= num_quads; ++i) {
    for (int j = 0; j <= num_quads; ++j) {
      const float x = j * kStepSize - 1.0f;
      const float z = i * kStepSize - 1.0f;
      const float y = 0.0f;
      points.push_back({.point = glm::vec3(x, y, z)});
    }
  }

  // Generate EBO indices from points
  const unsigned KVertsPerSide = num_quads + 1;
  std::vector<TriangleIdcs> eboIdcs;
  for (int i = 0; i < num_quads; ++i) {
    for (int j = 0; j < num_quads; ++j) {
      const unsigned p0 = i * KVertsPerSide + j;
      const unsigned p1 = (i + 1) * KVertsPerSide + j;
      const unsigned p2 = (i + 1) * KVertsPerSide + j + 1;
      const unsigned p3 = i * KVertsPerSide + j + 1;
      // Triangle 1
      eboIdcs.push_back({p0, p1, p2});
      // Triangle 2
      eboIdcs.push_back({p2, p3, p0});
    }
  }

  return {points, eboIdcs};
}
