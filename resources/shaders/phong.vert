#version 330 core

layout(location = 0) in vec3 positionObj;
layout(location = 1) in vec3 normalObj;

out vec3 position;
out vec3 normal;
out vec2 uv;
uniform mat4 modelmatrix;
uniform mat4 viewmatrix;
uniform mat4 projmatrix;

const float epsilon = 0.0001f;
const float PI = 3.1415926535897932384626433832795;
uniform int primitive; // !!!

vec2 findUVCoordinate(int primitive, vec3 intersectPosObj) {

    float u = 0;
    float v = 0;
    // PrimitiveType::PRIMITIVE_CUBE --0
    if (primitive == 0) {
        if (intersectPosObj[0] == 0.5) {
            u = -intersectPosObj[2] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
        else if (intersectPosObj[0] < -0.5+epsilon) {
            u = 0.5 + intersectPosObj[2];
            v = 0.5 + intersectPosObj[1];
        }
        else if (intersectPosObj[1] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = -intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[1] < -0.5+epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[2] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
        else if (intersectPosObj[2] < -0.5+epsilon) {
            u = -intersectPosObj[0] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
    }
    // PrimitiveType::PRIMITIVE_CYLINDER --2 || PrimitiveType::PRIMITIVE_CONE --1
    else if (primitive == 2 || primitive == 1) {
        if (intersectPosObj[1] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = -intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[1] < -0.5+epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[2] + 0.5;
        } else {
            v = intersectPosObj[1] + 0.5;
            float theta = atan(intersectPosObj[2], intersectPosObj[0]);
            theta < 0 ? u = -theta/(2.f*PI) : u = 1 - theta/(2.f*PI);
        }
    }
    // PrimitiveType::PRIMITIVE_SPHERE --3
    else if (primitive == 3) {
        if (intersectPosObj[1] == -0.5 || intersectPosObj[1] == 0.5) u = 0.5;
        else {
            float theta = atan(intersectPosObj[2], intersectPosObj[0]);
            theta < 0 ? u = -theta/(2.f*PI) : u = 1 - theta/(2.f*PI);
        }
        v = asin(intersectPosObj[1]*2)/PI + 0.5f;
    }

    return vec2(u, v);
}

void main() {
    uv = findUVCoordinate(primitive, positionObj);
    position = vec3(modelmatrix * vec4(positionObj, 1));
    mat3 smallModelmatrix = mat3(vec3(modelmatrix[0]), vec3(modelmatrix[1]), vec3(modelmatrix[2]));
    normal = normalize(inverse(transpose(smallModelmatrix)) * normalObj);
    gl_Position = projmatrix * viewmatrix * modelmatrix * vec4(positionObj, 1.0f);
}
