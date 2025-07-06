#version 330 core
in vec3 vColor;
in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    FragColor = texColor * vec4(vColor, 1.0);
}
