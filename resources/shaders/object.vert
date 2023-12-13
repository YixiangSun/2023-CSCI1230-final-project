#version 330 core

layout(location = 0) in vec3 positionObj;
layout(location = 1) in vec3 normalObj;

out vec3 position;
out vec3 normal;
out vec4 lightSpacePos; // !!!!!!!!!!!!!!!!!!!!!!!

uniform mat4 viewmatrix;
uniform mat4 projmatrix;
uniform mat4 gLightWVP; // !!!!!!!!!!!!!!!!!!!!!!!

void main() {
    position = positionObj / 2;
    normal = normalize(normalObj);
    gl_Position = projmatrix * viewmatrix * vec4(position, 1.0f);

    lightSpacePos = gLightWVP * vec4(position, 1.0f);
}
