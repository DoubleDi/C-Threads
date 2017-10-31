#version 330

in vec4 point;
in vec3 position;
in vec2 texture;

out vec2 TexCoord;

uniform mat4 camera;

void main() {
    mat4 positionMatrix = mat4(1.0);
    positionMatrix[3][0] = position.x;
    positionMatrix[3][1] = position.z;
    positionMatrix[3][2] = position.y;
    gl_Position = camera * positionMatrix * point;
    TexCoord = texture;
}
