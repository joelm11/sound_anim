#include "glm/ext/vector_float3.hpp"
#include <vector>

struct TriangleIdcs {
  unsigned a, b, c;
};

struct PlaneMeshData {
  std::vector<glm::vec3> verts;
  std::vector<TriangleIdcs> indices;
};

PlaneMeshData GenerateSquarePlane(const int num_quads);
