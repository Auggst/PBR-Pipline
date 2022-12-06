#version 430 core

in VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
} fs_in;

out vec4 FragColor;

uniform vec3 lightColor;

void main() {
    vec3 albedo = lightColor;

    // HDR tonemapping
    vec3 color = albedo / (albedo + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}