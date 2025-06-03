#include "uniforms/src/uniforms.hh"
#include <filesystem>
#include <glad/glad.h>
#include <ostream>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/imgui/src/jimgui.hh"
#include "init_routines.hh"
#include "lib/mesh.hh"
#include "shaders/shader_m.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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

  JimGUI::initImGUI(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // build and compile our shader zprogram
  // ------------------------------------
  const auto cwd = std::filesystem::current_path();
  const std::string vsPath = "src/shaders/waves.vs";
  const std::string fsPath = "src/shaders/waves.fs";
  Shader ourShader(cwd / vsPath, cwd / fsPath);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  const PlaneMeshData kMesh = GenerateSquarePlane(1024);
  unsigned int vert_buffer, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &vert_buffer);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
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

  // Set rendering params and MVP uniforms.
  glParams();
  ourShader.use();
  Uniforms::initUniforms(ourShader);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    ourShader.setFloat("u_time", glfwGetTime());
    JimGUI::prepNewFrame();
    Uniforms::setWaveParamsUniforms(ourShader, Uniforms::genWaveParams());

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render container
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, kMesh.indices.size() * 3, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    JimGUI::renderFrame();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &vert_buffer);
  JimGUI::shutdown();
  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
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