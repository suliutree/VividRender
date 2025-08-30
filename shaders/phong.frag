#version 330 core
in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uAlbedo;
uniform vec3 uLightDir;      // 归一化世界方向
uniform vec3 uViewPos;

out vec4 FragColor;

void main() {
    vec3 ambient = 0.3 * texture(uAlbedo, vUV).rgb;

    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * texture(uAlbedo, vUV).rgb;

    vec3 V = normalize(uViewPos - vFragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 32.0);
    vec3 specular = vec3(0.4) * spec;   // 固定高光色

    FragColor = vec4(ambient + diffuse + specular, 1.0);

    // FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
