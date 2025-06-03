#pragma once
#include "shader_m.h"
#include <glm/glm.hpp>
#include <vector>

namespace Uniforms {
struct ViewParams {
  glm::mat4 model, view, projection;
};

struct WaveParams {
  float amplitude;
  float frequency;
  float phase;
  glm::vec2 direction;
};

struct LightingParams {
  glm::vec3 lightpos;
  glm::vec3 camerapos;
};

// Putting the uniform data generating functions here for now
ViewParams genViewParamsStatic(const int scWidth, const int scHeight,
                               const glm::vec3 cameraPos);

std::vector<WaveParams> genWaveParams();

LightingParams genLightingParamsStatic(const glm::vec3 cameraPos);

// Functions to set uniform data
void setViewParamsUniforms(const Shader &shader, const ViewParams &params);

void setWaveParamsUniforms(const Shader &shader,
                           const std::vector<WaveParams> &params);

void setLightingParamsUniforms(const Shader &shader,
                               const LightingParams &params);
} // namespace Uniforms