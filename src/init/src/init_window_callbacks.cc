#include "glm/common.hpp"
#include "glm/trigonometric.hpp"
#include "init_routines.hh"

// Camera control globals
float g_azimuth = glm::radians(45.0f);   // horizontal angle
float g_elevation = glm::radians(30.0f); // vertical angle
float g_radius = 8.0f;                   // distance from origin
float g_lastX = 0.0f, g_lastY = 0.0f;
bool g_dragging = false;

namespace InitRoutines {
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      g_dragging = true;
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      g_lastX = (float)xpos;
      g_lastY = (float)ypos;
    } else if (action == GLFW_RELEASE) {
      g_dragging = false;
    }
  }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  if (g_dragging) {
    float dx = (float)xpos - g_lastX;
    float dy = (float)ypos - g_lastY;
    g_lastX = (float)xpos;
    g_lastY = (float)ypos;
    float sensitivity = 0.01f;
    g_azimuth -= dx * sensitivity;
    g_elevation -= dy * sensitivity;
    // Clamp elevation to avoid flipping
    float min_elev = glm::radians(-89.0f);
    float max_elev = glm::radians(89.0f);
    g_elevation = glm::clamp(g_elevation, min_elev, max_elev);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  g_radius -= (float)yoffset * 0.5f;
  if (g_radius < 0.3f)
    g_radius = 0.3f;
  if (g_radius > 50.0f)
    g_radius = 50.0f;
}

GLFWwindow *initWindowCallbacks(GLFWwindow *window) {
  // Register camera controls
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, scroll_callback);
  return window;
}
} // namespace InitRoutines