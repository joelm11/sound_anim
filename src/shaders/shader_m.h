#ifndef SHADER_H
#define SHADER_H

#include <cstddef>
#include <filesystem>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
  unsigned int ID;
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader(const std::filesystem::path &vertexPath,
         const std::filesystem::path &fragmentPath);
  // activate the shader
  // ------------------------------------------------------------------------
  void use() const;
  // utility uniform functions
  // ------------------------------------------------------------------------
  void setBool(const std::string &name, bool value) const;
  // ------------------------------------------------------------------------
  void setInt(const std::string &name, int value) const;
  // ------------------------------------------------------------------------
  void setFloat(const std::string &name, float value) const;
  void setFloatArray(const std::string &name, const float *values,
                     size_t count) const;
  // ------------------------------------------------------------------------
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec2Array(const std::string &name, const glm::vec2 *values,
                    const size_t count) const;
  void setVec2(const std::string &name, float x, float y) const;
  // ------------------------------------------------------------------------
  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;
  // ------------------------------------------------------------------------
  void setVec3Array(const std::string &name, const glm::vec3 *values,
                    size_t count) const;
  // ------------------------------------------------------------------------
  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setVec4(const std::string &name, float x, float y, float z,
               float w) const;
  // ------------------------------------------------------------------------
  void setMat2(const std::string &name, const glm::mat2 &mat) const;
  // ------------------------------------------------------------------------
  void setMat3(const std::string &name, const glm::mat3 &mat) const;
  // ------------------------------------------------------------------------
  void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(unsigned shader, std::string type);
};
#endif