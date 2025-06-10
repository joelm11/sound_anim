#include "uniforms.hh"
#include <cmath>
#include <filesystem>
#include <glad/glad.h>
#include <memory>
#include <ostream>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/imgui/src/jimgui.hh"
#include "init_routines.hh"
#include "skybox/skybox.hh"
#include "waves/wave_shader.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Camera controls (cleanup later?)
extern float g_azimuth;
extern float g_elevation;
extern float g_radius;
extern float g_lastX;
extern bool g_dragging;

void processInput(GLFWwindow *window);
void glParams() {
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
}

int main() {
  GLFWwindow *window = InitRoutines::initWindow(720, 546);

  InitRoutines::initWindowCallbacks(window);

  // JimGUI::initImGUI(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // build and compile our shader zprogram
  // ------------------------------------
  const auto cwd = std::filesystem::current_path();
  const std::string vsPathSB = "src/shaders/skybox/skybox.vs";
  const std::string fsPathSB = "src/shaders/skybox/skybox.fs";
  const std::string vsPathWave = "src/shaders/waves/waves.vs";
  const std::string fsPathWave = "src/shaders/waves/waves.fs";
  auto skyboxShader =
      std::make_unique<SkyboxShader>(cwd / vsPathSB, cwd / fsPathSB);
  auto wavesShader =
      std::make_unique<WaveShader>(cwd / vsPathWave, cwd / fsPathWave);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // Clear the frame
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate camera position from spherical coordinates
    float x = g_radius * std::sin(g_elevation) * std::sin(g_azimuth);
    float y = g_radius * std::cos(g_elevation);
    float z = g_radius * std::sin(g_elevation) * std::cos(g_azimuth);
    glm::vec3 camPos = glm::vec3(x, y, z);
    // Update uniforms with new camera position
    const Uniforms::ViewParams vparams =
        Uniforms::genViewParamsStatic(720, 550, camPos);
    skyboxShader->use();
    skyboxShader->setUniform("u_view", vparams.view);
    skyboxShader->draw();

    wavesShader->use();
    wavesShader->setUniform("u_view", vparams.view);
    wavesShader->setUniform("u_camerapos", camPos);
    wavesShader->setUniform("u_time", glfwGetTime());
    // Pass the skybox texture to the wave shader
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxShader->textureID);
    // wavesShader->setUniform("u_skyboxTexture", 1);
    wavesShader->draw();

    processInput(window);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse
    // moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // JimGUI::shutdown();
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}