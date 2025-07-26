#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vColor;
out vec2 vTexCoord;

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
