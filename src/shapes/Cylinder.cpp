#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cylinder::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    std::vector<float> addOn = {};
    if (topLeft[1] == 0.5 && topRight[1] == 0.5 && bottomRight[1] == 0.5 && bottomLeft[1] == 0.5) {
        addOn = {
            topLeft[0], topLeft[1], topLeft[2],
            0, 0.5, 0,
            bottomLeft[0], bottomLeft[1], bottomLeft[2],
            0, 0.5, 0,
            bottomRight[0], bottomRight[1], bottomRight[2],
            0, 0.5, 0,
            bottomRight[0], bottomRight[1], bottomRight[2],
            0, 0.5, 0,
            topRight[0], topRight[1], topRight[2],
            0, 0.5, 0,
            topLeft[0], topLeft[1], topLeft[2],
            0, 0.5, 0
        };
    }
    else if (topLeft[1] == -0.5 && topRight[1] == -0.5 && bottomRight[1] == -0.5 && bottomLeft[1] == -0.5) {
        addOn = {
            topLeft[0], topLeft[1], topLeft[2],
            0, -0.5, 0,
            bottomLeft[0], bottomLeft[1], bottomLeft[2],
            0, -0.5, 0,
            bottomRight[0], bottomRight[1], bottomRight[2],
            0, -0.5, 0,
            bottomRight[0], bottomRight[1], bottomRight[2],
            0, -0.5, 0,
            topRight[0], topRight[1], topRight[2],
            0, -0.5, 0,
            topLeft[0], topLeft[1], topLeft[2],
            0, -0.5, 0
        };
    }
    else {
        glm::vec3 normal1 = glm::normalize(glm::vec3(topLeft[0], 0, topLeft[2]));
        glm::vec3 normal2 = glm::normalize(glm::vec3(topRight[0], 0, topRight[2]));
        addOn = {
            topLeft[0], topLeft[1], topLeft[2],
            normal1[0], normal1[1], normal1[2],
            bottomLeft[0], bottomLeft[1], bottomLeft[2],
            normal1[0], normal1[1], normal1[2],
            bottomRight[0], bottomRight[1], bottomRight[2],
            normal2[0], normal2[1], normal2[2],
            bottomRight[0], bottomRight[1], bottomRight[2],
            normal2[0], normal2[1], normal2[2],
            topRight[0], topRight[1], topRight[2],
            normal2[0], normal2[1], normal2[2],
            topLeft[0], topLeft[1], topLeft[2],
            normal1[0], normal1[1], normal1[2],
        };
    }
    for (auto item : addOn) {
        m_vertexData.push_back(item);
    }
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    for (int i = 0; i < m_param1; i++) {
        glm::vec3 tR1 = {
            glm::cos(currentTheta)*float(i)/float(m_param1)*m_radius,
            0.5,
            glm::sin(currentTheta)*float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 tL1 = {
                  glm::cos(nextTheta)*float(i)/float(m_param1)*m_radius,
                  0.5,
                  glm::sin(nextTheta)*float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 dR1 = {
            glm::cos(currentTheta)*float(i+1)/float(m_param1)*m_radius,
            0.5,
            glm::sin(currentTheta)*float(i+1)/float(m_param1)*m_radius,
        };
        glm::vec3 dL1 = {
            glm::cos(nextTheta)*float(i+1)/float(m_param1)*m_radius,
            0.5,
            glm::sin(nextTheta)*float(i+1)/float(m_param1)*m_radius,
        };

        glm::vec3 tL2 = {tR1[0], -0.5, tR1[2]};
        glm::vec3 tR2 = {tL1[0], -0.5, tL1[2]};
        glm::vec3 dL2 = {dR1[0], -0.5, dR1[2]};
        glm::vec3 dR2 = {dL1[0], -0.5, dL1[2]};

        glm::vec3 tR3 = {
            glm::cos(currentTheta)*m_radius,
            0.5 - float(i)/float(m_param1),
            glm::sin(currentTheta)*m_radius,
        };
        glm::vec3 tL3 = {
            glm::cos(nextTheta)*m_radius,
            0.5 - float(i)/float(m_param1),
            glm::sin(nextTheta)*m_radius,
        };
        glm::vec3 dR3 = {
            glm::cos(currentTheta)*m_radius,
            0.5 - float(i+1)/float(m_param1),
            glm::sin(currentTheta)*m_radius,
        };
        glm::vec3 dL3 = {
            glm::cos(nextTheta)*m_radius,
            0.5 - float(i+1)/float(m_param1),
            glm::sin(nextTheta)*m_radius,
        };
        makeTile(tL1, tR1, dL1, dR1);
        makeTile(tL2, tR2, dL2, dR2);
        makeTile(tL3, tR3, dL3, dR3);
    }
}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float step = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = step * i;
        float nextTheta = step * (i+1);
        makeWedge(currentTheta, nextTheta);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
