#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include <filesystem>
#include <ostream>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/imgui/src/jimgui.hh"
#include "lib/mesh.hh"
#include "shaders/shader_m.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb/stb_image.h>

// settings
const unsigned int SCR_WIDTH = 720;
const unsigned int SCR_HEIGHT = 546;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void glParams() {
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Enable depth test
  // glEnable(GL_DEPTH_TEST);

  // // Accept fragment if it closer to the camera than the former one
  // glDepthFunc(GL_LESS);
}

// Camera orbit parameters
float cameraYaw = glm::pi<float>() / 2.0f;   // azimuth, horizontal angle
float cameraPitch = glm::pi<float>() / 2.0f; // elevation, vertical angle
float cameraRadius = 5.0f;                   // fixed distance from origin

// Mouse drag state
bool mouseDragging = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      mouseDragging = true;
      glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    } else if (action == GLFW_RELEASE) {
      mouseDragging = false;
    }
  }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!mouseDragging)
    return;
  double dx = xpos - lastMouseX;
  double dy = ypos - lastMouseY;
  lastMouseX = xpos;
  lastMouseY = ypos;

  // Sensitivity factors
  float sensitivity = 0.005f;
  cameraYaw -= dx * sensitivity;
  cameraPitch -= dy * sensitivity;

  // Clamp pitch to avoid flipping
  float epsilon = 0.01f;
  cameraPitch = glm::clamp(cameraPitch, epsilon, glm::pi<float>() - epsilon);
}

void setMVP(const Shader shader) {
  glm::mat4 model, view, projection;
  model = view = projection = glm::mat4(1.0f);

  // Camera position in spherical coordinates
  float x = cameraRadius * sin(cameraPitch) * cos(cameraYaw);
  float y = cameraRadius * cos(cameraPitch);
  float z = cameraRadius * sin(cameraPitch) * sin(cameraYaw);
  glm::vec3 camPos = glm::vec3(x, y, z);
  view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // Constant.
  projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

  // We set these uniforms once as our view is constant for now.
  shader.setMat4("u_model", model);
  shader.setMat4("u_view", view);
  shader.setMat4("u_projection", projection);
}

void setWaveParamUniforms(const Shader shader) {
  static const int kNumSines = 3;
  static float kAmps[kNumSines] = {0.07, 0.09, 0.04};
  static float kFreqs[kNumSines] = {0.65, 1.0, 0.65};
  static float kPhases[kNumSines] = {1.7, glm::pi<float>(), 1.7};
  static glm::vec2 kDirs[kNumSines] = {{1.0, 0.0}, {0.0, 1.0}, {0.7, 0.7}};
  // -- -Fixed - size C - style array of floats(e.g., float[5])-- -
  ImGui::Text("Float Array (Sliders):");
  for (int i = 0; i < kNumSines; ++i) {
    // Push a unique ID for each widget.
    // Using the loop index 'i' is common and usually sufficient.
    ImGui::PushID(i);
    ImGui::SliderFloat("Value", &kPhases[i], 0.0f, 5.0f, "%.2f");
    ImGui::PopID(); // Pop the ID to avoid it affecting subsequent widgets
                    // outside this loop
  }
  ImGui::Separator();

  shader.setFloatArray("u_amplitudes", kAmps, kNumSines);
  shader.setFloatArray("u_frequencies", kFreqs, kNumSines);
  shader.setFloatArray("u_phases", kPhases, kNumSines);
  shader.setVec2Array("u_wavedirs", kDirs, kNumSines);
}

void setLightingUniforms(const Shader shader) {
  shader.setVec3("u_lightpos", 0.0, 1.0, 0.0);
  shader.setVec3("u_camerapos", 0.0, 5.0, 0.0);
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
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
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
  const PlaneMeshData kMesh = GenerateSquarePlane(64);
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
  setMVP(ourShader);
  // setWaveParamUniforms(ourShader);
  setLightingUniforms(ourShader);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    JimGUI::prepNewFrame();
    setWaveParamUniforms(ourShader);
    ourShader.setFloat("u_time", glfwGetTime());

    // input
    // -----
    processInput(window);

    // Update camera/view each frame in case of drag
    setMVP(ourShader);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render container
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, kMesh.indices.size() * 3, GL_UNSIGNED_INT, 0);

    JimGUI::renderFrame();
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}