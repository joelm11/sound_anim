#version 300 es
layout(location = 0) in vec3 aPos;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 TexCoords;

void main() {
    TexCoords = aPos;
    vec4 pos = u_projection * u_view * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}
