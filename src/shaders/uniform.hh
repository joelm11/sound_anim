#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Uniform {
// Dispatch helper
template <typename T> struct UniformDispatcher {
  static void set(const int shaderId, const std::string name, const T &data) {
    static_assert(
        sizeof(T) == 0,
        "No matching Shader::setUniform implementation for this type.");
  }
};

// Scalar specializations
template <> struct UniformDispatcher<const bool &> {
  static void set(const int shaderId, const std::string name, bool data) {
    glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int)data);
  }
};

template <> struct UniformDispatcher<const int &> {
  static void set(const int shaderId, const std::string name, int data) {
    glUniform1i(glGetUniformLocation(shaderId, name.c_str()), data);
  }
};

template <> struct UniformDispatcher<const float &> {
  static void set(const int shaderId, const std::string name, float data) {
    glUniform1f(glGetUniformLocation(shaderId, name.c_str()), data);
  }
};

template <> struct UniformDispatcher<const double &> {
  static void set(const int shaderId, const std::string name, float data) {
    glUniform1f(glGetUniformLocation(shaderId, name.c_str()), data);
  }
};

template <> struct UniformDispatcher<const glm::vec2 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::vec2 &data) {
    glUniform2fv(glGetUniformLocation(shaderId, name.c_str()), 1, &data[0]);
  }
};

template <> struct UniformDispatcher<const glm::vec3 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::vec3 &data) {
    glUniform3fv(glGetUniformLocation(shaderId, name.c_str()), 1, &data[0]);
  }
};

template <> struct UniformDispatcher<const glm::vec4 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::vec4 &data) {
    glUniform4fv(glGetUniformLocation(shaderId, name.c_str()), 1, &data[0]);
  }
};

template <> struct UniformDispatcher<const glm::mat2 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::mat2 &data) {
    glUniformMatrix2fv(glGetUniformLocation(shaderId, name.c_str()), 1, 0,
                       &data[0][0]);
  }
};

template <> struct UniformDispatcher<const glm::mat3 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::mat3 &data) {
    glUniformMatrix3fv(glGetUniformLocation(shaderId, name.c_str()), 1, 0,
                       &data[0][0]);
  }
};

template <> struct UniformDispatcher<const glm::mat4 &> {
  static void set(const int shaderId, const std::string name,
                  const glm::mat4 &data) {
    glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, 0,
                       &data[0][0]);
  }
};

// Array specializations
template <> struct UniformDispatcher<const std::vector<float> &> {
  static void set(const int shaderId, const std::string name,
                  const std::vector<float> &data) {
    glUniform1fv(glGetUniformLocation(shaderId, name.c_str()), data.size(),
                 data.data());
  }
};

template <> struct UniformDispatcher<const std::vector<glm::vec2> &> {
  static void set(const int shaderId, const std::string name,
                  const std::vector<glm::vec2> &data) {
    glUniform2fv(glGetUniformLocation(shaderId, name.c_str()), data.size(),
                 &data[0][0]);
  }
};

// --- Base Class for all Uniforms (now handling value storage) ---
class UniformBase {
public:
  std::string name;
  // This needs to list ALL possible types a uniform can hold.
  using UniformValue = std::variant<bool, int, float, double, glm::vec2,
                                    glm::vec3, glm::vec4, glm::mat3, glm::mat4,
                                    std::vector<float>, std::vector<glm::vec2>>;

public:
  UniformBase(const std::string &uniformName, UniformValue initialData,
              const int shaderId)
      : kShaderId(shaderId), name(uniformName), m_data(initialData) {}

  // A generic setData method that takes any of the variant types
  template <typename T> void setData(T newData) { m_data = newData; }

  // `apply` method uses std::visit to dispatch based on the stored type
  void apply() const {
    std::visit(
        [&](const auto &arg) {
          UniformDispatcher<decltype(arg)>::set(kShaderId, name, arg);
        },
        m_data);
  }

  virtual ~UniformBase() = default; // Still need virtual destructor

private:
  const int kShaderId;
  UniformValue m_data; // Store the data as a variant
};

} // namespace Uniform
