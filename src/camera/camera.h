#pragma once

#include "utils/scenedata.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
    SceneCameraData cameraData;
    int width, height;

public:
    Camera(const SceneCameraData cameraData, int width, int height);
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.

    // Gets the sceneCameraData of the camera.
    SceneCameraData getData();

    // Updates the sceneCameraData of the camera.
    void updateData(SceneCameraData cData);

    // Updates the width and height of the camera.
    void updateWH(int newWidth, int newHeight);

    // Returns the view matrix of the camera.
    glm::mat4 getViewMatrix();

    // Returns the projection matrix of the camera.
    glm::mat4 getProjMatrix();

    glm::mat4 InitCameraTransform(glm::vec3 Pos, glm::vec3 Look, glm::vec3 Up);
    glm::mat4 InitOrthoProjTransform(float l, float r, float b, float t, float n, float f);

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
};
