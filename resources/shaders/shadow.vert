#version 330 core

//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

//out vec3 FragPos;  // directly pass FragPos without a struct
//out vec3 Normal;
//out vec2 TexCoords;
//out vec4 FragPosLightSpace;

uniform mat4 lightprojection;
uniform mat4 lightview;
uniform mat4 model;
//uniform mat4 lightSpaceMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

void main()
{
//    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = transpose(inverse(mat3(model))) * aNormal;
//    TexCoords = aTexCoords;
//    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = lightprojection * lightview * model * vec4(aPos, 1.0);
}
