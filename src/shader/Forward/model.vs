#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aTitangent;

out VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.TexCoords = aTexCoords;
	vs_out.Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(vs_out.Position, 1.0);

    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aTitangent);
    vec3 N = normalize(normalMatrix * aNormal);    
    
    mat3 TBN = transpose(mat3(T, B, N));  
    vs_out.TBN = TBN;
}