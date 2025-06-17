#pragma once
#include "mesh.hh"
#include "shader.hh"
#include <OpenGL/OpenGL.h>
#include <filesystem>

struct WaveParams {
  std::vector<float> amplitudes;
  std::vector<float> frequencies;
  std::vector<float> speeds;
  std::vector<glm::vec2> directions;
};

class WaveShader : public ShaderProgram {
public:
  WaveShader(const std::filesystem::path &vertexPath,
             const std::filesystem::path &fragmentPath);
  ~WaveShader();

  virtual void initBuffers() override;
  virtual void draw() override;

private:
  void initUniforms();
  const PlaneMeshData kMesh = GenerateSquarePlane(1024);
  unsigned vertexBufferData, VAO, EBO;
};