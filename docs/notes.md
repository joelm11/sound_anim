## Drawing Notes

### Vertex Buffer Object (VBO)

- Buffer containing vertex data (positions, colors, texture coordinates) to be used by the GPU. VBOs are transferred to the GPU then kept there.

### Vertex Array Object (VAO)

- Stores the configuration of vertex attribute pointers.
- It keeps track of VBOs and their associated vertex attribute configurations (e.g., which data belongs to positions, normals, or texture coordinates).
- By storing the configuration of the VBO, one VAO can be used to draw all the vertices in a VBO.

### Element Buffer Object (EBO)

- Stores indices that define how to connect vertices to form geometric shapes like triangles, lines, or quads.
- Reduces the number of vertices required to draw shapes.

### What I need

1. Initially a unit cube with the speaker positions.
2. Need transforms that give 3 views - side, top, back, iso.

#### Constants

- Known screen size for each view.
- Room doesn't change, just camera placement. Room may be slightly scaled for different views and some walls are not drawn.

### Development Procedure

1. Draw unit cube with a different colour on each face.
2. Move camera to view cube from different views.
3. Adjust views to match dimensions given in drawings.
4. Add textures to cube walls.
5. Add models to cube views.
6. Add speaker placements in unit cube.
7. Attempt to draw speakers for each room view.
