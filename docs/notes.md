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

### Drawing rasterized waves

## What I need

- Generate points on a mesh
- Generate EBO indices for mesh points
- Render mesh as a solid surface with triangles.
- Implement some sort of simple lighting to start.
- Implement a more interesting wave function.
