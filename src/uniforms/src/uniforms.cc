#include "uniforms.hh"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include <random>

namespace Uniforms {

void initUniforms(const Shader &shader) {
  const glm::vec3 camPos = {0.0, 5.0, -5.0};
  Uniforms::setLightingParamsUniforms(
      shader, Uniforms::genLightingParamsStatic(camPos));
  Uniforms::setViewParamsUniforms(
      shader, Uniforms::genViewParamsStatic(720, 550, camPos));
  Uniforms::setWaveParamsUniforms(shader, Uniforms::genWaveParams());
}

ViewParams genViewParamsStatic(const int scWidth, const int scHeight,
                               const glm::vec3 cameraPos) {
  ViewParams params;
  params.model = params.view = params.projection = glm::mat4(1.0f);

  params.view =
      glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 4.0f, 0.0f));

  // Constant.
  params.projection = glm::perspective(
      glm::radians(45.0f), (float)scWidth / (float)scHeight, 0.1f, 100.0f);
  return params;
}

#define NUM_WAVES 8
std::vector<WaveParams> genWaveParams() {
  std::vector<WaveParams> params;
  params.reserve(NUM_WAVES); // Pre-allocate memory for efficiency

  // Random number generation setup
  // Use a high-quality random number generator
  std::mt19937 rng(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  // Define distributions for each parameter
  // Amplitude: e.g., 0.1 to 1.0 (smaller waves to larger waves)
  std::uniform_real_distribution<float> amplitudeDist(0.1f, 0.8f);
  // Frequency: e.g., 0.5 to 3.0 (slower to faster oscillations)
  std::uniform_real_distribution<float> frequencyDist(0.1f, 1.0f);
  // Phase: e.g., 0 to 2*PI (random starting point in wave cycle)
  std::uniform_real_distribution<float> phaseDist(
      0.0f, glm::two_pi<float>()); // glm::two_pi is 2 * PI
  // Direction: angle in radians, then converted to a unit vector
  std::uniform_real_distribution<float> directionAngleDist(
      0.0f, glm::two_pi<float>());

  for (int i = 0; i < NUM_WAVES; ++i) {
    WaveParams wave;

    // Generate random values for each parameter
    wave.amplitude = amplitudeDist(rng);
    wave.frequency = frequencyDist(rng);
    wave.phase = phaseDist(rng);

    // Generate a random direction vector
    float angle = directionAngleDist(rng);
    wave.direction = glm::vec2(std::cos(angle), std::sin(angle));
    // Optional: Normalize just to be safe, though cos/sin of angle should
    // produce unit vector
    wave.direction = glm::normalize(wave.direction);

    params.push_back(wave);
  }

  return params;
}

LightingParams genLightingParamsStatic(const glm::vec3 cameraPos) {
  return {.camerapos = cameraPos, .lightpos = {0.0, 1.0, 0.0}};
}

void setViewParamsUniforms(const Shader &shader, const ViewParams &params) {
  shader.setMat4("u_model", params.model);
  shader.setMat4("u_view", params.view);
  shader.setMat4("u_projection", params.projection);
}

void setWaveParamsUniforms(const Shader &shader,
                           const std::vector<WaveParams> &params) {
  std::vector<float> amp, freq, phase;
  std::vector<glm::vec2> dir;
  for (const WaveParams param : params) {
    amp.push_back(param.amplitude);
    freq.push_back(param.frequency);
    phase.push_back(param.phase);
    dir.push_back(param.direction);
  }

  shader.setFloatArray("u_amplitudes", amp.data(), amp.size());
  shader.setFloatArray("u_frequencies", freq.data(), freq.size());
  shader.setFloatArray("u_phases", phase.data(), phase.size());
  shader.setVec2Array("u_wavedirs", dir.data(), dir.size());
}

void setLightingParamsUniforms(const Shader &shader,
                               const LightingParams &params) {
  shader.setVec3("u_lightpos", params.lightpos);
  shader.setVec3("u_camerapos", params.camerapos);
}
} // namespace Uniforms