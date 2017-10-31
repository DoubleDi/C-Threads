#version 330
in vec4 point;
in vec3 position;
in vec4 variance;
in vec2 texture;

uniform mat4 camera;
out vec2 TexCoord;

void main() {
    mat4 scaleMatrix = mat4(1.0);
    mat4 rotateMatrix = mat4(0.0);
    rotateMatrix[0][0] = cos(gl_InstanceID);
    rotateMatrix[1][1] = 1.0;
    rotateMatrix[0][2] = sin(gl_InstanceID);
    rotateMatrix[2][0] = -sin(gl_InstanceID);
    rotateMatrix[2][2] = cos(gl_InstanceID);
    rotateMatrix[3][3] = 1.0;
    scaleMatrix[0][0] = 0.01 * (abs(sin(gl_InstanceID)) < 0.8 ? 0.8 : abs(sin(gl_InstanceID)));
    scaleMatrix[1][1] = 0.1 * (1 - abs(variance.x) * 4.0) * (abs(sin(gl_InstanceID)) < 0.8 ? 0.8 : abs(sin(gl_InstanceID)));
    mat4 positionMatrix = mat4(1.0);
    positionMatrix[3][0] = position.x;
    positionMatrix[3][1] = pow(position.z, 0.9) / 9.0;
    positionMatrix[3][1] *= position.z < 0 ? 1 : -1;
    positionMatrix[3][1] -= position.y < 0.45 && position.x >= 0.8 && position.x < 1 ? 0.02 : 0;
    positionMatrix[3][1] -= position.y >= 0.1 && position.y < 0.3 && position.x >= 0.7 && position.x < 1 ? 0.03 : 0;
    positionMatrix[3][1] -= position.y < 0.6 && position.x >= 1 && position.x < 1.1 ? 0.01 : 0;
    positionMatrix[3][2] = position.y;
    if (position.z < 0.05 && position.z >= -1.2 && (position.x > 0.1 || position.y > 0.1)) {
        gl_Position = camera * (positionMatrix * rotateMatrix * scaleMatrix * (point - vec4(0, 0.1 ,0,0)) + (variance * pow(point.y, 2)));
        TexCoord = texture;
    }
}
