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
