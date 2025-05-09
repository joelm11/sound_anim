#include "glm/ext/vector_float3.hpp"
#include <vector>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "shader_m.h"
#include "speaker_points/speaker_points.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 720;
const unsigned int SCR_HEIGHT = 546;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void glParams() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Enable depth test
  // glEnable(GL_DEPTH_TEST);

  // // Accept fragment if it closer to the camera than the former one
  // glDepthFunc(GL_LESS);
}
void setMVP(const Shader shader) {
  glm::mat4 model, view, projection;
  model = view = projection = glm::mat4(1.0f);

  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

  // Constant.
  projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

  // We set these uniforms once as our view is constant for now.
  shader.setMat4("u_model", model);
  shader.setMat4("u_view", view);
  shader.setMat4("u_projection", projection);
}

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLProgram", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // build and compile our shader zprogram
  // ------------------------------------
  Shader ourShader("/Users/joelm/Desktop/joelgl 2/src/room.vs",
                   "/Users/joelm/Desktop/joelgl 2/src/room.fs");
  std::cout << "Built shaders\n";

  // Calculate and set speaker uniform source positions
  // const std::vector<glm::vec3> spkrPos = {
  //     sphericalToCartesian(1.f, 0.f, 30.f),
  //     sphericalToCartesian(1.f, 0.f, -30.f)};
  const std::vector<glm::vec3> spkrPos = {{0.f, 1.f, 0.f}};
  ourShader.setVec3Array("u_spkrPos[NUM_SPKRS]", spkrPos.data(),
                         spkrPos.size());

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  const int kNumPoints = 1000;
  std::vector<glm::vec3> spherePoints =
      generateFibonacciSpherePoints(kNumPoints);

  unsigned int sphere_vert_buffer, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &sphere_vert_buffer);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, sphere_vert_buffer);
  glBufferData(GL_ARRAY_BUFFER, spherePoints.size() * sizeof(glm::vec3),
               spherePoints.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Set constants before loop like rendering params, MVP uniforms, and activate
  // shader.
  glParams();
  ourShader.use();
  setMVP(ourShader);
  std::cout << "Finished init\n";

  const double kStartTime = glfwGetTime(); // Get the start time
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // calculate delta time
    double currentTime = glfwGetTime();
    float time = static_cast<float>(currentTime - kStartTime);
    ourShader.setFloat("u_time", time); // Set the time uniform

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render container
    glBindVertexArray(VAO);
    glPointSize(7.f);
    glDrawArrays(GL_POINTS, 0, kNumPoints);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &sphere_vert_buffer);

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

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}