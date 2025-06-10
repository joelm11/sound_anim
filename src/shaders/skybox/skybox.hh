#include "mesh.hh"
#include "shader.hh"

struct SkyboxVertex {
  glm::vec3 pos;
};

class SkyboxShader : public ShaderProgram {
public:
  SkyboxShader(const std::filesystem::path &vertexPath,
               const std::filesystem::path &fragmentPath)
      : ShaderProgram(vertexPath, fragmentPath) {}

private:
  virtual void initBuffers() override;

  virtual void draw() override;

  std::array<SkyboxVertex, 8> verts_;
  std::array<TriangleIdcs, 12> idcs_;
  unsigned vertexBufferData, vao, ebo;
};