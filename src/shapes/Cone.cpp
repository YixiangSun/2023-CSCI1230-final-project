#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    glm::vec3 normal1, normal2, normalTop;
    for (int i = 0; i < m_param1; i++) {
        glm::vec3 tL1 = {
            glm::cos(currentTheta)*float(i)/float(m_param1)*m_radius,
            -0.5,
            glm::sin(currentTheta)*float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 tR1 = {
            glm::cos(nextTheta)*float(i)/float(m_param1)*m_radius,
            -0.5,
            glm::sin(nextTheta)*float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 dL1 = {
            glm::cos(currentTheta)*float(i+1)/float(m_param1)*m_radius,
            -0.5,
            glm::sin(currentTheta)*float(i+1)/float(m_param1)*m_radius,
        };
        glm::vec3 dR1 = {
            glm::cos(nextTheta)*float(i+1)/float(m_param1)*m_radius,
            -0.5,
            glm::sin(nextTheta)*float(i+1)/float(m_param1)*m_radius,
        };

        glm::vec3 tR2 = {
            glm::cos(currentTheta) * float(i)/float(m_param1)*m_radius,
            0.5 - float(i)/float(m_param1),
            glm::sin(currentTheta) * float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 tL2 = {
            glm::cos(nextTheta) * float(i)/float(m_param1)*m_radius,
            0.5 - float(i)/float(m_param1),
            glm::sin(nextTheta) * float(i)/float(m_param1)*m_radius,
        };
        glm::vec3 dR2 = {
            glm::cos(currentTheta) * float(i+1)/float(m_param1)*m_radius,
            0.5 - float(i+1)/float(m_param1),
            glm::sin(currentTheta) * float(i+1)/float(m_param1)*m_radius,
        };
        glm::vec3 dL2 = {
            glm::cos(nextTheta) * float(i+1)/float(m_param1)*m_radius,
            0.5 - float(i+1)/float(m_param1),
            glm::sin(nextTheta) * float(i+1)/float(m_param1)*m_radius,
        };

        float theta = (currentTheta + nextTheta)/2.0f;
        normalTop = glm::normalize(glm::vec3(std::cos(theta), 0.5, std::sin(theta)));
        normal1 = glm::normalize(glm::vec3(std::cos(currentTheta), 0.5, std::sin(currentTheta)));
        normal2 = glm::normalize(glm::vec3(std::cos(nextTheta), 0.5, std::sin(nextTheta)));

        std::vector<float> addOn = {};
        glm::vec3 normal3, normal4;
        if (i == 0) {
            normal3 = normalTop;
            normal4 = normalTop;
        }
        else {
            normal3 = normal1;
            normal4 = normal2;
        };
        addOn = {
            tL1[0], tL1[1], tL1[2],
            0, -0.5, 0,
            dL1[0], dL1[1], dL1[2],
            0, -0.5, 0,
            dR1[0], dR1[1], dR1[2],
            0, -0.5, 0,
            dR1[0], dR1[1], dR1[2],
            0, -0.5, 0,
            tR1[0], tR1[1], tR1[2],
            0, -0.5, 0,
            tL1[0], tL1[1], tL1[2],
            0, -0.5, 0,
            tL2[0], tL2[1], tL2[2],
            normal4[0], normal4[1], normal4[2],
            dL2[0], dL2[1], dL2[2],
            normal2[0], normal2[1], normal2[2],
            dR2[0], dR2[1], dR2[2],
            normal1[0], normal1[1], normal1[2],
            dR2[0], dR2[1], dR2[2],
            normal1[0], normal1[1], normal1[2],
            tR2[0], tR2[1], tR2[2],
            normal3[0], normal3[1], normal3[2],
            tL2[0], tL2[1], tL2[2],
            normal4[0], normal4[1], normal4[2],
        };
        for (auto item : addOn) {
            m_vertexData.push_back(item);
        }

    }
}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float step = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = step * i;
        float nextTheta = step * (i+1);
        makeWedge(currentTheta, nextTheta);
    }
}
