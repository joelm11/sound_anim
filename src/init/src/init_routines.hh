#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

namespace InitRoutines {
GLFWwindow *initWindow(const int scWidth, const int scHeight);
GLFWwindow *initWindowCallbacks(GLFWwindow *window);
} // namespace InitRoutines
