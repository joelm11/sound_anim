#include "skybox.hh"

void SkyboxShader::initBuffers() {
  // Generate vertices on the unit cube.
  verts_ = {
      SkyboxVertex{{-0.5f, -0.5f, -0.5f}}, SkyboxVertex{{0.5f, -0.5f, -0.5f}},
      SkyboxVertex{{0.5f, 0.5f, -0.5f}},   SkyboxVertex{{-0.5f, 0.5f, -0.5f}},
      SkyboxVertex{{-0.5f, -0.5f, 0.5f}},  SkyboxVertex{{0.5f, -0.5f, 0.5f}},
      SkyboxVertex{{0.5f, 0.5f, 0.5f}},    SkyboxVertex{{-0.5f, 0.5f, 0.5f}}};

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

void SkyboxShader::draw() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, idcs_.size() * 3, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}