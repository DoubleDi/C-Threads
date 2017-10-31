#version 330

in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D outTexture;

void main() {
    outColor = texture(outTexture, TexCoord);
}
