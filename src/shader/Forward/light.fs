#version 430 core

in VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
} fs_in;

out vec4 FragColor;

uniform vec3 lightColor;
uniform sampler2D diffuse;

void main() {
    // vec3 albedo = vec3(0.001) + pow(texture(diffuse, fs_in.TexCoords).rgb, vec3(2.2));

    vec3 albedo = lightColor;

    // HDR tonemapping
    vec3 color = albedo / (albedo + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}