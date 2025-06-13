#pragma once
#include "mesh.hh"
#include "shader.hh"
#include <OpenGL/OpenGL.h>
#include <filesystem>

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