#include "skybox.hh"
#include "uniforms.hh"
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void SkyboxShader::initBuffers() {
  // Generate vertices on the unit cube.
  const int kCubeWidth = 8;
  verts_ = {SkyboxVertex{{-kCubeWidth, -kCubeWidth, -kCubeWidth}},
            SkyboxVertex{{kCubeWidth, -kCubeWidth, -kCubeWidth}},
            SkyboxVertex{{kCubeWidth, kCubeWidth, -kCubeWidth}},
            SkyboxVertex{{-kCubeWidth, kCubeWidth, -kCubeWidth}},
            SkyboxVertex{{-kCubeWidth, -kCubeWidth, kCubeWidth}},
            SkyboxVertex{{kCubeWidth, -kCubeWidth, kCubeWidth}},
            SkyboxVertex{{kCubeWidth, kCubeWidth, kCubeWidth}},
            SkyboxVertex{{-kCubeWidth, kCubeWidth, kCubeWidth}}};

  // Generate triangle indices on the unit cube.
  idcs_ = {TriangleIdcs(0, 1, 2), TriangleIdcs(2, 3, 0), TriangleIdcs(4, 5, 6),
           TriangleIdcs(6, 7, 4), TriangleIdcs(0, 3, 7), TriangleIdcs(7, 4, 0),
           TriangleIdcs(1, 2, 6), TriangleIdcs(6, 5, 1), TriangleIdcs(0, 1, 5),
           TriangleIdcs(5, 4, 0), TriangleIdcs(3, 2, 6), TriangleIdcs(6, 7, 3)};

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vertexBufferData);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData);
  glBufferData(GL_ARRAY_BUFFER, verts_.size() * sizeof(SkyboxVertex),
               verts_.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkyboxVertex::pos),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idcs_.size() * sizeof(TriangleIdcs),
               idcs_.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void SkyboxShader::initUniforms() {
  // Set VS uniforms
  Uniforms::ViewParams vparams =
      Uniforms::genViewParamsStatic(720, 550, {0.0, 1.0, -5.0});
  addUniform("u_model", vparams.model);
  addUniform("u_view", vparams.view);
  addUniform("u_projection", vparams.projection);

  addUniform("u_camerapos", glm::vec3(0.0, 1.0, -5.0));

  addUniform("u_skyboxTexture", 0);

  for (const auto &val : uniforms_) {
    val.second->apply();
  }
}

void SkyboxShader::loadTexture() {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  const auto cwd = std::filesystem::current_path();
  const std::string texPath("resources/skybox.png");
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(std::filesystem::path(cwd / texPath).c_str(),
                                  &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load skybox texture" << std::endl;
    stbi_image_free(data);
    return;
  }

  // Assuming the image is arranged as a cross (horizontal layout)
  int faceWidth = width / 3;
  int faceHeight = height / 2;

  // Define the order of faces in the image
  std::vector<glm::ivec2> facePositions = {
      // Right, left, top, bottom, front, back
      {2, 0}, {0, 0}, {1, 1}, {0, 1}, {1, 0}, {2, 1},
  };

  for (unsigned int i = 0; i < 6; ++i) {
    int x = facePositions[i].x * faceWidth;
    int y = facePositions[i].y * faceHeight;

    // Extract the face data
    unsigned char *faceData =
        new unsigned char[faceWidth * faceHeight * nrChannels];
    for (int row = 0; row < faceHeight; ++row) {
      for (int col = 0; col < faceWidth; ++col) {
        for (int channel = 0; channel < nrChannels; ++channel) {
          faceData[(row * faceWidth + col) * nrChannels + channel] =
              data[((y + (faceHeight - 1 - row)) * width + (x + col)) *
                       nrChannels +
                   channel];
        }
      }
    }

    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, faceWidth,
                 faceHeight, 0, format, GL_UNSIGNED_BYTE, faceData);
    delete[] faceData;
  }

  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyboxShader::draw() {
  use();
  glClearColor(0.4f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthMask(GL_FALSE);

  glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, idcs_.size() * 3, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind texture
  glDepthMask(GL_TRUE);
}