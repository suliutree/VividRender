#version 330 core
in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D uTex;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTex, vTexCoord);
    FragColor = texColor * vec4(vColor, 1.0);
}
