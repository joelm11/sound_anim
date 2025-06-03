#include "uniforms.hh"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Uniforms {
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

std::vector<WaveParams> genWaveParams() {
  // We'll actually populate some values here after we've found some nice
  // combinations with the gui
  return {};
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