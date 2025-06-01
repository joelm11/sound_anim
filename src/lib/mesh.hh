#include "glm/ext/vector_float3.hpp"
#include <vector>

struct Vertex {
  glm::vec3 point;
  glm::vec3 normal;
};

struct TriangleIdcs {
  unsigned a, b, c;
};

struct PlaneMeshData {
  std::vector<Vertex> verts;
  std::vector<TriangleIdcs> indices;
};

PlaneMeshData GenerateSquarePlane(const int num_quads);
