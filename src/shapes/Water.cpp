#include "Water.h"
#include <iostream>
#include <random>

// Generate time offset list

void Water::updateParams(int rowCount, int colCount, float height, float timeWave, bool initialized) {
    m_vertexData = std::vector<float>();
    m_rowCount = rowCount;
    m_colCount = colCount;
    m_height = height;
    m_wavetime = timeWave;
    if (!initialized) {
        for (int i = 0; i < (rowCount+1) * (colCount+1); i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> distribution(0, 2.0*M_PI);
            m_timeOffsets.push_back(distribution(gen));
        }
    }
    setVertexData();
}

const float waveLength = 4.0;
const float waveAmplitude = 0.2;

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
    float yDistortion = generateOffset(vertex.x, vertex.z, 0.1, 0.3, waveTime);
    //    return vertex + glm::vec3(xDistortion, yDistortion, zDistortion);
    return vertex + glm::vec3(vertex.x, yDistortion, vertex.z);
}
// !!!!! //
void Water::makeWaterTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight) {

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
    glm::vec3 stepZ = (bottomLeft - topLeft) / float(m_colCount);

    for (int row = 0; row < m_rowCount; row++) {
        for (int col = 0; col < m_colCount; col++) {
            // Calculate the corner points for each tile without using glm::mix
            glm::vec3 tileTopLeft = topLeft + stepX * (float) row + stepZ * (float) col;
            float timeOffset1 = m_timeOffsets[row * (m_colCount+1) + col];
            glm::vec3 tileTopRight = tileTopLeft + stepX;
            float timeOffset2 = m_timeOffsets[row * (m_colCount+1) + col + (m_colCount + 1)];
            glm::vec3 tileBottomLeft = tileTopLeft + stepZ;
            float timeOffset3 = m_timeOffsets[row * (m_colCount+1) + col + 1];
            glm::vec3 tileBottomRight = tileTopLeft + stepX + stepZ;
            float timeOffset4 = m_timeOffsets[row * (m_colCount+1) + col + (m_colCount + 1) + 1];

            tileTopLeft = applyDistortion(tileTopLeft, m_wavetime + timeOffset1);
            tileTopRight = applyDistortion(tileTopRight, m_wavetime + timeOffset2);
            tileBottomLeft = applyDistortion(tileBottomLeft, m_wavetime + timeOffset3);
            tileBottomRight = applyDistortion(tileBottomRight, m_wavetime + timeOffset4);

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
