#include "Water.h"

void Water::updateParams(int rowCount, int colCount, float height, float timeWave) {
    m_vertexData = std::vector<float>();
    m_rowCount = rowCount;
    m_colCount = colCount;
    m_height = height;
    m_wavetime = timeWave;
    setVertexData();
}

const float waveLength = 4.0;
const float waveAmplitude = 0.2; // 0.2

glm::vec3 calcNormal(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2) {
    glm::vec3 tangent = vertex1 - vertex0;
    glm::vec3 bitangent = vertex2 - vertex0;
    return glm::normalize(glm::cross(tangent, bitangent));
}

float generateOffset(float x, float z, float val1, float val2, float waveTime) {
    float radiansX = ((std::fmod(x + z * x * val1, waveLength) / waveLength) + waveTime * std::fmod(x * 0.8 + z, 1.5)) * 2.0 * M_PI;
    float radiansZ = ((std::fmod(val2 * (z * x + x * z), waveLength) / waveLength) + waveTime * 2.0 * std::fmod(x, 2.0)) * 2.0 * M_PI;
    return waveAmplitude * 0.5 * (std::sin(radiansZ) + std::cos(radiansX));
}

glm::vec3 applyDistortion(glm::vec3 vertex, float waveTime) {
    //    float xDistortion = generateOffset(vertex.x, vertex.z, 0.2, 0.1, waveTime);
    float yDistortion = generateOffset(vertex.x, vertex.z, 0.1, 0.3, waveTime);
    //    float zDistortion = generateOffset(vertex.x, vertex.z, 0.15, 0.2, waveTime);
    //    return vertex + glm::vec3(xDistortion, yDistortion, zDistortion);
    return vertex + glm::vec3(vertex.x, yDistortion, vertex.z);
}
// !!!!! //
void Water::makeWaterTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere

    glm::vec3 Ex1 = calcNormal(topLeft, bottomLeft, bottomRight);
    // Triangle 1 (top-left, bottom-left, bottom-right)
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, Ex1);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, Ex1);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, Ex1);

    glm::vec3 Ex2 = calcNormal(bottomRight, topRight, topLeft);
    // Triangle 2 (bottom-right, top-right, top-left)
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, Ex2);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, Ex2);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, Ex2);
}

void Water::makeFace(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight) {

    glm::vec3 stepX = (topRight - topLeft) / float(m_rowCount);
    glm::vec3 stepY = (bottomLeft - topLeft) / float(m_colCount);

    for (int row = 0; row < m_rowCount; row++) {
        for (int col = 0; col < m_colCount; col++) {
            // Calculate the corner points for each tile without using glm::mix
            glm::vec3 tileTopLeft = topLeft + stepX * (float) row + stepY * (float) col;
            glm::vec3 tileTopRight = tileTopLeft + stepX;
            glm::vec3 tileBottomLeft = tileTopLeft + stepY;
            glm::vec3 tileBottomRight = tileTopLeft + stepX + stepY;

            tileTopLeft = applyDistortion(tileTopLeft, m_wavetime);
            tileTopRight = applyDistortion(tileTopRight, m_wavetime);
            tileBottomLeft = applyDistortion(tileBottomLeft, m_wavetime);
            tileBottomRight = applyDistortion(tileBottomRight, m_wavetime);

            // Create a tile for the current position
            makeWaterTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight);
        }
    }
}

void Water::setVertexData() {

    makeFace(glm::vec3(-0.5f, m_height, -0.5f),
             glm::vec3( 0.5f, m_height, -0.5f),
             glm::vec3(-0.5f, m_height,  0.5f),
             glm::vec3( 0.5f, m_height,  0.5f));
}

void Water::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
