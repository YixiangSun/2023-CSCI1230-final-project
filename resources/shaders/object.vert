#version 330 core

layout(location = 0) in vec3 positionObj;
layout(location = 1) in vec3 normalObj;

out vec3 position;
out vec3 normal;

uniform mat4 viewmatrix;
uniform mat4 projmatrix;

void main() {
    position = positionObj / 3;
    normal = normalize(normalObj);
    gl_Position = projmatrix * viewmatrix * vec4(position, 1.0f);
}
