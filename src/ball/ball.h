#ifndef BALL_H
#define BALL_H

#include "utils/scenedata.h"
#include <iostream>
#include <glm/glm.hpp>
#endif // BALL_H

class Ball {
    glm::mat4 ctm = glm::mat4(1.0f);
    double r = 0.3;
    SceneMaterial material {
        .cAmbient =  SceneColor(glm::vec4(1, 1, 1, 1)),  // Ambient term
        .cDiffuse = SceneColor(glm::vec4(1, 1, 1, 1)),  // Diffuse term
        .cSpecular = SceneColor(glm::vec4(0, 0, 0, 0)), // Specular term
        .shininess = 0,      // Specular exponent

        .cReflective = SceneColor(glm::vec4(0, 0, 0, 0)), // Used to weight contribution of reflected ray lighting (via multiplication)
        .blend = 0.5
    };

public:
    Ball(glm::mat4 ctm, float r, SceneMaterial material);

    // Restart ball
    void restart();

    // Gets the ctm of the ball.
    glm::mat4 getCTM();

    // Updates the ctm of the camera.
    void updateCTM(glm::mat4 ctm);

    // Gets the radius of the ball.
    float getRadius();

    // Gets the material of the ball.
    SceneMaterial getMaterial();

    // Gets the position of the ball.
    glm::vec4 getPos();

    void changeMaterial(SceneMaterial newMaterial);

};
