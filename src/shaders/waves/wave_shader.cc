#include "wave_shader.hh"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include "uniforms.hh"
#include <glm/vec2.hpp>
#include <vector>

WaveShader::WaveShader(const std::filesystem::path &vertexPath,
                       const std::filesystem::path &fragmentPath)
    : ShaderProgram(vertexPath, fragmentPath) {
  use();
  initBuffers();
  initUniforms();
}

WaveShader::~WaveShader() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &vertexBufferData);
}

void WaveShader::initBuffers() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &vertexBufferData);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData);
  glBufferData(GL_ARRAY_BUFFER, kMesh.verts.size() * sizeof(Vertex),
               kMesh.verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               kMesh.indices.size() * sizeof(TriangleIdcs),
               kMesh.indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void WaveShader::draw() {
  use();

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, kMesh.indices.size() * 3, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void WaveShader::initUniforms() {
  // Set VS uniforms
  const glm::vec3 kCameraPos(0.0, 5.0, -5.0);
  Uniforms::ViewParams vparams =
      Uniforms::genViewParamsStatic(720, 550, kCameraPos);
  addUniform("u_model", vparams.model);
  addUniform("u_view", vparams.view);
  addUniform("u_projection", vparams.projection);
  addUniform("u_time", 0);
  // Wave params
  const unsigned kNumWaves = 8;
  auto params = generateWaveParams(kNumWaves);
  addUniform("u_amps", params.amplitudes);
  addUniform("u_freqs", params.frequencies);
  addUniform("u_dirs", params.directions);

  // Set FS uniforms
  addUniform("u_lightPos", glm::vec3(0.0, 5.0, 0.0));
  addUniform("u_viewPos", kCameraPos);

  // Apply uniforms
  for (const auto &val : uniforms_) {
    val.second->apply();
  }
}

WaveParams WaveShader::generateWaveParams(const int numWaves) {
  std::vector<float> amps, freqs;
  std::vector<glm::vec2> dirs;

  const float kSpeed = 0.5;
  const float kFreq = glm::two_pi<float>() * 1.0;
  const float kMedianWavelength = kSpeed / kFreq;
  const float kWLRatio = 0.2;

  const float windAngle = glm::radians(30.0f);
  glm::vec2 windDir = glm::vec2(std::cos(windAngle), std::sin(windAngle));
  const float maxAngleOffset = glm::radians(20.0f);

  for (int i = 0; i < numWaves; ++i) {
    float angleOffset =
        ((float)rand() / RAND_MAX) * 2.0f * maxAngleOffset - maxAngleOffset;
    float waveAngle = windAngle + angleOffset;
    glm::vec2 direction = glm::vec2(std::cos(waveAngle), std::sin(waveAngle));

    float wavelength = kMedianWavelength * (0.5f + ((float)rand() / RAND_MAX));
    float amplitude = kWLRatio * wavelength;
    float frequency = kSpeed / wavelength;
    float phase = ((float)rand() / RAND_MAX) * glm::two_pi<float>();

    amps.push_back(amplitude);
    freqs.push_back(frequency);
    dirs.push_back(direction);
  }
  return {amps, freqs, dirs};
}
