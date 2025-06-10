#pragma once
#include "uniform.hh"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

class ShaderProgram {
public:
  // Deriving classes shall configure their own vertex buffers and data
  // attributes.
  virtual void initBuffers() = 0;

  // Deriving classes shall implement their own draw methods.
  virtual void draw() = 0;

  ShaderProgram(const std::filesystem::path &vertexPath,
                const std::filesystem::path &fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      vShaderFile.close();
      fShaderFile.close();
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
                << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    shaderId_ = glCreateProgram();
    glAttachShader(shaderId_, vertex);
    glAttachShader(shaderId_, fragment);
    glLinkProgram(shaderId_);
    checkCompileErrors(shaderId_, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  void use() { glUseProgram(shaderId_); }

  // Adds a uniform and sets its default value in the shader program.
  template <typename T>
  void addUniform(const std::string &uniformName, T value) {
    uniforms_[uniformName] =
        std::make_unique<Uniform::UniformBase>(uniformName, value, shaderId_);
  }

  // Sets the value of an added uniform. Throws for invalid uniforms.
  template <typename T>
  void setUniform(const std::string &uniformName, T value) {
    if (!uniforms_.contains(uniformName))
      throw std::runtime_error("ShaderProgram::Uniform not found");
    uniforms_[uniformName]->setData(value);
    uniforms_[uniformName]->apply();
  }

  std::unordered_map<std::string, std::unique_ptr<Uniform::UniformBase>>
      uniforms_;
  unsigned shaderId_;

private:
  void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
        std::terminate();
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
        std::terminate();
      }
    }
  }
};