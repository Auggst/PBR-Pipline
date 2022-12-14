#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.Position = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(vs_out.Position, 1.0);
    
	
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    //vs_out.Normal = aNormal;
}