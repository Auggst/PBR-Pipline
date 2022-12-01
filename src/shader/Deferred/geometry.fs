#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in VS_OUT{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main() {
    gPosition = fs_in.Position;
    gNormal = normalize(fs_in.Normal);
    gColorSpec.rgb = pow(texture(material.texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2));
    gColorSpec.a = texture(material.texture_specular1, fs_in.TexCoords).r;
}
