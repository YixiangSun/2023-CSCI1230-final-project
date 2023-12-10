
#include "ball.h"
#include "glm/ext/matrix_transform.hpp"


Ball::Ball(glm::mat4 ctm, float r, SceneMaterial material)
    : ctm(ctm), r(r), material(material) {
}


// Gets the ctm of the ball.
glm::mat4 Ball::getCTM() {
    return ctm;
}

// Updates the ctm of the camera.
void Ball::updateCTM(glm::mat4 newCtm) {
    ctm = newCtm;
}

// Gets the radius of the ball.
float Ball::getRadius() {
    return r;
}

SceneMaterial Ball::getMaterial() {
    return material;
}

glm::vec4 Ball::getPos() {
    return ctm * glm::vec4(0.000f, 0.00f, 0.000f, 1.000f);
}

void Ball::restart() {
    ctm = glm::translate(glm::mat4(1.0f), glm::vec3(0, r, 0));
}

void Ball::changeMaterial(SceneMaterial newMaterial) {
    material = newMaterial;
}
