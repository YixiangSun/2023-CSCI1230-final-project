#include "Cube.h"
#include <iostream>
#include <ostream>

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::cross((bottomLeft-topLeft), (bottomRight-topLeft)));
    glm::vec3 normal2 = glm::normalize(glm::cross((topRight-bottomRight), (topLeft-bottomRight)));
    std::vector<float> addOn = {
        topLeft[0], topLeft[1], topLeft[2],
        normal1[0], normal1[1], normal1[2],
        bottomLeft[0], bottomLeft[1], bottomLeft[2],
        normal1[0], normal1[1], normal1[2],
        bottomRight[0], bottomRight[1], bottomRight[2],
        normal1[0], normal1[1], normal1[2],
        bottomRight[0], bottomRight[1], bottomRight[2],
        normal2[0], normal2[1], normal2[2],
        topRight[0], topRight[1], topRight[2],
        normal2[0], normal2[1], normal2[2],
        topLeft[0], topLeft[1], topLeft[2],
        normal2[0], normal2[1], normal2[2],
    };
    for (auto item : addOn) {
        m_vertexData.push_back(item);
    }
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            glm::vec3 interval0 = -(topLeft - topRight) / float(m_param1);
            glm::vec3 interval1 = -(topLeft - bottomLeft) / float(m_param1);
            glm::vec3 tL = topLeft + float(i)*interval0 + float(j)*interval1;
            glm::vec3 tR = tL + interval0;
            glm::vec3 bL = tL + interval1;
            glm::vec3 bR = tL + interval0 + interval1;
            makeTile(tL, tR, bL, bR);
        }
    }
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube


}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

//     makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
//              glm::vec3( 0.5f,  0.5f, 0.5f),
//              glm::vec3(-0.5f, -0.5f, 0.5f),
//              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:

     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
     makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f));

     makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
              glm::vec3( 0.5f, 0.5f, -0.5f),
              glm::vec3(-0.5f, 0.5f, 0.5f),
              glm::vec3( 0.5f, 0.5f, 0.5f));

     makeFace(glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3(0.5f,  0.5f, 0.5f),
              glm::vec3(0.5f, -0.5f, -0.5f),
              glm::vec3(0.5f,  0.5f, -0.5f));

     makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3(-0.5f, -0.5f,  0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));

     makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
