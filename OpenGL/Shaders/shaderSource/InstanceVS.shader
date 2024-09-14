#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;
out vec3 fragPos;
out vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool IsPlanet;



void main()
{
    TexCoords = aTexCoords;
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;
    if(IsPlanet)
        gl_Position = projection * view * model * vec4(aPos, 1.0f); 
    else
        gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0f); 
}