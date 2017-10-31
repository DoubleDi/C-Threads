#version 330

in vec4 point;
in vec3 position;
in vec2 texture;

out vec2 TexCoord;

uniform mat4 camera;

void main() {
    vec4 h_point = point;
    h_point.x += position.x;
    h_point.z += position.z;
    h_point.y += position.y;
    gl_Position = camera * h_point;
    TexCoord = texture;
}
