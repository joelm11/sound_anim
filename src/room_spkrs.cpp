#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "room_vertices.hpp"
#include "speaker_vertices.hpp"
#include "shader_m.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build shader program.
    Shader ourShader("/Users/joelm/Downloads/joelgl 2/src/spkr.vs", "/Users/joelm/Downloads/joelgl 2/src/spkr.fs");

    // Define vertices and indices
std::vector<glm::vec3> spkr_verts = {
    glm::vec3(-0.5f, 0.0f, -1.0f),
    glm::vec3(0.5f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.8f, -1.0f),
};

const float norm_offset = 0.06f;
auto front_speaker_coords = kfront_spkrs;
auto front_speaker_norm = glm::vec3(norm_offset, 0.f, 0.f);
auto front_speaker_verts = spkr_from_coord(front_speaker_coords.data(), front_speaker_coords.size(), front_speaker_norm);

auto side_speaker_coords = kside_spkrs;
auto side_speaker_norm = glm::vec3(0.f, 0.f, norm_offset);
auto side_speaker_verts = spkr_from_coord(side_speaker_coords.data(), side_speaker_coords.size(), side_speaker_norm);

auto rear_speaker_coords = krear_spkrs;
auto rear_speaker_norm = glm::vec3(norm_offset, 0.f, 0.f);
auto rear_speaker_verts = spkr_from_coord(rear_speaker_coords.data(), rear_speaker_coords.size(), rear_speaker_norm);

// Combine all speaker vertices into one vector
std::vector<glm::vec3> all_speaker_verts;
all_speaker_verts.reserve(front_speaker_verts.size() + side_speaker_verts.size() + rear_speaker_verts.size());
all_speaker_verts.insert(all_speaker_verts.end(), front_speaker_verts.begin(), front_speaker_verts.end());
all_speaker_verts.insert(all_speaker_verts.end(), side_speaker_verts.begin(), side_speaker_verts.end());
all_speaker_verts.insert(all_speaker_verts.end(), rear_speaker_verts.begin(), rear_speaker_verts.end());

std::array<unsigned int, 6> spkr_vert_idx = 
{   0, 1, 2, 
    1, 2, 3,
};
std::vector<unsigned int> all_spkr_vert_idx;
// Fill the vector with multiple copies of spkr_vert_idx
for (int i = 0; i < (kfront_spkrs.size() + kside_spkrs.size() + krear_spkrs.size()); ++i) {
    for (int j = 0; j < spkr_vert_idx.size(); ++j) {
        all_spkr_vert_idx.push_back(spkr_vert_idx[j] + i * 4);
    }
}

// Generate and bind VAO, VBO, and EBO
unsigned int VAO, VBO, EBO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, all_speaker_verts.size() * sizeof(glm::vec3), all_speaker_verts.data(), GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spkr_vert_idx) * all_spkr_vert_idx.size(), all_spkr_vert_idx.data(), GL_STATIC_DRAW);

glm::mat4 model      = glm::mat4(1.0f);
glm::mat4 view       = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

// Top-view. (Red Green Green Red)
model = glm::scale(model, glm::vec3(1.0f, 1.f, 1.3f));
view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)) * glm::rotate(view, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

// Print projection matrix.
std::cout << "Projection matrix: " << std::endl;
for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
        std::cout << projection[i][j] << " ";
    }
    std::cout << std::endl;
}

// Main render loop
while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);   
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader program
    ourShader.use();
    ourShader.setMat4("model", model);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // Set speaker colour.
    double  timeValue = glfwGetTime();
    float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
    ourShader.setVec3("lightColour", glm::vec3(0.0f, greenValue, 0.0f));

    // Bind VAO and draw
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, all_spkr_vert_idx.size(), GL_UNSIGNED_INT, 0);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}