#include <iostream>
#include <ostream>
#include <stdexcept>
#include "camera.h"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(const SceneCameraData cameraData, int width, int height)
    : cameraData(cameraData), width(width), height(height) {
    // You can also perform any additional initialization if needed.
}
SceneCameraData Camera::getData()   {
    return cameraData;
}

void Camera::updateWH(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void Camera::updateData(SceneCameraData cData) {
    cameraData = cData;
}

glm::mat4 Camera::getViewMatrix()  {
    glm::vec3 w = -glm::normalize(cameraData.look);
    glm::vec3 v = glm::normalize(glm::vec3(cameraData.up) - glm::dot(glm::vec3(cameraData.up), w)*w);
    glm::vec3 u = glm::cross(v, w);
    glm::mat4 r = glm::mat4({u[0], v[0], w[0], 0.f}, {u[1], v[1], w[1], 0.f}, {u[2], v[2], w[2], 0.f}, {0, 0, 0, 1.f});
    glm::mat4 t = glm::translate(glm::mat4(1.0f), -glm::vec3(cameraData.pos));
    return r*t;
}

glm::mat4 InitTranslationTransform(float x, float y, float z)
{
    glm::mat4 m;
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    return m;
}


glm::mat4 InitTranslationTransform(glm::vec3 Pos)
{
    return InitTranslationTransform(Pos.x, Pos.y, Pos.z);
}


glm::mat4 InitRotateTrans(glm::vec3 Look, glm::vec3 Up)
{
    glm::vec3 N = normalize(Look);
    glm::vec3 UpNorm = normalize(Up);
    glm::vec3 U = normalize(cross(UpNorm, N));
    glm::vec3 V = cross(N, U);

    glm::mat4 m;
    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;

    return m;
}


glm::mat4 Camera::InitCameraTransform(glm::vec3 Pos, glm::vec3 Look, glm::vec3 Up)
{
    glm::mat4 CameraTranslation = InitTranslationTransform(-Pos.x, -Pos.y, -Pos.z);

    glm::mat4 CameraRotateTrans = InitRotateTrans(Look, Up);

    return CameraRotateTrans * CameraTranslation;
}


glm::mat4 Camera::getProjMatrix()  {
    // Optional TODO: implement the getter or make your own design
    float c = -0.1f / 200.0f;
    glm::mat4 unhingingMap = glm::mat4(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f/(1.f+c), -1.f),
        glm::vec4(0.f, 0.f, -c/(1.f+c), 0.f)
    );
    float theta_h = cameraData.heightAngle;
    glm::mat4 scalingMap = glm::mat4(
        glm::vec4(1.f/(200.0f * tan(theta_h/2)*width/height), 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f/(200.0f * tan(theta_h/2)), 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f/200.0f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    glm::mat4 translationMap = glm::mat4(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, -2.f, 0.f),
        glm::vec4(0.f, 0.f, -1.f, 1.f)
    );
    return translationMap * unhingingMap * scalingMap;

}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    return float(width)/float(height);
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData.heightAngle;
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData.focalLength;
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData.aperture;
}

glm::mat4 Camera::InitOrthoProjTransform(float l, float r, float b,
                                         float t, float n, float f)
{
    glm::mat4 m;
    m[0][0] = 2.0f/(r - l); m[0][1] = 0.0f;         m[0][2] = 0.0f;         m[0][3] = -(r + l)/(r - l);
    m[1][0] = 0.0f;         m[1][1] = 2.0f/(t - b); m[1][2] = 0.0f;         m[1][3] = -(t + b)/(t - b);
    m[2][0] = 0.0f;         m[2][1] = 0.0f;         m[2][2] = 2.0f/(f - n); m[2][3] = -(f + n)/(f - n);
    m[3][0] = 0.0f;         m[3][1] = 0.0f;         m[3][2] = 0.0f;         m[3][3] = 1.0;

    return m;
}
