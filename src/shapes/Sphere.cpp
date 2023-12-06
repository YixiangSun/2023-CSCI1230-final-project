#include "Sphere.h"

void Sphere::updateParams(int param1, int param2, float radius) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    m_radius = radius;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    glm::vec3 nomal1 = glm::normalize(glm::vec3(topLeft[0], topLeft[1], topLeft[2]));
    glm::vec3 nomal2 = glm::normalize(glm::vec3(bottomLeft[0], bottomLeft[1], bottomLeft[2]));
    glm::vec3 nomal3 = glm::normalize(glm::vec3(bottomRight[0], bottomRight[1], bottomRight[2]));
    glm::vec3 nomal4 = glm::normalize(glm::vec3(topRight[0], topRight[1], topRight[2]));
    std::vector<float> addOn = {
        topLeft[0], topLeft[1], topLeft[2],
        nomal1[0], nomal1[1], nomal1[2],
        bottomLeft[0], bottomLeft[1], bottomLeft[2],
        nomal2[0], nomal2[1], nomal2[2],
        bottomRight[0], bottomRight[1], bottomRight[2],
        nomal3[0], nomal3[1], nomal3[2],
        bottomRight[0], bottomRight[1], bottomRight[2],
        nomal3[0], nomal3[1], nomal3[2],
        topRight[0], topRight[1], topRight[2],
        nomal4[0], nomal4[1], nomal4[2],
        topLeft[0], topLeft[1], topLeft[2],
        nomal1[0], nomal1[1], nomal1[2],
    };
    for (auto item : addOn) {
        m_vertexData.push_back(item);
    }
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float phiStep = glm::radians(180.f / m_param1);
    for (int i = 0; i < m_param1; i++) {
        float currentPhi = phiStep*i;
        float nextPhi = phiStep*(i+1);
        glm::vec3 bL = glm::vec3(glm::sin(currentPhi)*glm::cos(currentTheta)*m_radius, glm::cos(currentPhi)*m_radius, glm::sin(currentPhi)*glm::sin(currentTheta)*m_radius);
        glm::vec3 bR = glm::vec3(glm::sin(currentPhi)*glm::cos(nextTheta)*m_radius, glm::cos(currentPhi)*m_radius, glm::sin(currentPhi)*glm::sin(nextTheta)*m_radius);
        glm::vec3 tL = glm::vec3(glm::sin(nextPhi)*glm::cos(currentTheta)*m_radius, glm::cos(nextPhi)*m_radius, glm::sin(nextPhi)*glm::sin(currentTheta)*m_radius);
        glm::vec3 tR = glm::vec3(glm::sin(nextPhi)*glm::cos(nextTheta)*m_radius, glm::cos(nextPhi)*m_radius, glm::sin(nextPhi)*glm::sin(nextTheta)*m_radius);
        if (currentPhi < M_PI) makeTile(tL, tR, bL, bR);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (1+i) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
    // Note: think about how param 2 comes into play here!
}

void Sphere::setVertexData() {

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
