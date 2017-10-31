#version 330

in vec4 point;
in vec2 texture;

out vec2 TexCoord;

uniform mat4 camera;

void main() {
    gl_Position = camera * point;
    TexCoord = texture;
}
