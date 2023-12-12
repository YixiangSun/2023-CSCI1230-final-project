#version 330 core

layout(location = 0) in vec3 positionObj;
layout(location = 1) in vec3 normalObj;

out vec3 position;
out vec3 normal;
//uniform mat4 modelmatrix;
uniform mat4 viewmatrix;
uniform mat4 projmatrix;

//const float epsilon = 0.0001f;
//const float PI = 3.1415926535897932384626433832795;
//uniform int primitive; // !!!

// correct !!!!

void main() {
    position = positionObj;
    normal = normalize(normalObj);
    gl_Position = projmatrix * viewmatrix * vec4(positionObj, 1.0f);
}
