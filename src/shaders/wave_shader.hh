#pragma once
#include "mesh.hh"
#include "shader.hh"
#include "uniforms.hh"

class WaveShader : public ShaderProgram {
public:
  WaveShader(const std::filesystem::path &vertexPath,
             const std::filesystem::path &fragmentPath)
      : ShaderProgram(vertexPath, fragmentPath) {
    use();
    initBuffers();
    initUniforms();
  }

  ~WaveShader() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBufferData);
  }

  virtual void initBuffers() override {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertexBufferData);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData);
    glBufferData(GL_ARRAY_BUFFER, kMesh.verts.size() * sizeof(Vertex),
                 kMesh.verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 kMesh.indices.size() * sizeof(TriangleIdcs),
                 kMesh.indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  virtual void draw() override {
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render container
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, kMesh.indices.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

private:
  void initUniforms() {
    // Set VS uniforms
    Uniforms::ViewParams vparams =
        Uniforms::genViewParamsStatic(720, 550, {0.0, 1.0, -5.0});
    addUniform("u_model", vparams.model);
    addUniform("u_view", vparams.view);
    addUniform("u_projection", vparams.projection);

    std::vector<Uniforms::WaveParams> wparams = Uniforms::genWaveParams();
    std::vector<float> amp, freq, phase;
    std::vector<glm::vec2> dir;
    for (const Uniforms::WaveParams param : wparams) {
      amp.push_back(param.amplitude);
      freq.push_back(param.frequency);
      phase.push_back(param.phase);
      dir.push_back(param.direction);
    }
    addUniform("u_amplitudes", amp);
    addUniform("u_frequencies", freq);
    addUniform("u_phases", phase);
    addUniform("u_wavedirs", dir);
    addUniform("u_time", 0);

    // Set FS uniforms
    Uniforms::LightingParams lparams =
        Uniforms::genLightingParamsStatic({0.0, 1.0, -5.0});
    addUniform("u_lightpos", lparams.lightpos);
    addUniform("u_camerapos", lparams.camerapos);

    // Apply uniforms
    for (const auto &val : uniforms_) {
      val.second->apply();
    }
  }

  const PlaneMeshData kMesh = GenerateSquarePlane(1024);
  unsigned vertexBufferData, VAO, EBO;
};