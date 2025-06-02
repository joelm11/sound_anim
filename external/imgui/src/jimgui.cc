#include "../imgui-src/backends/imgui_impl_glfw.h"
#include "../imgui-src/imgui.h"
#include "../imgui-src/imgui_impl_opengl3.h"

namespace ImGUI {
void initImGUI() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      YOUR_WINDOW, true); // Second param install_callback=true will install
                          // GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
}
} // namespace ImGUI