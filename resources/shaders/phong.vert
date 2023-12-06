#version 330 core

layout(location = 0) in vec3 positionObj;
layout(location = 1) in vec3 normalObj;

out vec3 position;
out vec3 normal;
uniform mat4 modelmatrix;
uniform mat4 viewmatrix;
uniform mat4 projmatrix;
void main() {
    position = vec3(modelmatrix * vec4(positionObj, 1));
    mat3 smallModelmatrix = mat3(vec3(modelmatrix[0]), vec3(modelmatrix[1]), vec3(modelmatrix[2]));
    normal = normalize(inverse(transpose(smallModelmatrix)) * normalObj);
    gl_Position = projmatrix * viewmatrix * modelmatrix * vec4(positionObj, 1.0f);
}
