#pragma once

#include <vector>
#include <glm/glm.hpp>

class Water
{
public:
    void updateParams(int rowCount, int colCount, float height, float timeWave, bool initialized);
    //    std::vector<float> generateShape() { return m_vertexData; }
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void setVertexData();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    //    void storeTriangle(glm::vec2 cornerPos[4], bool left);
    //    void makeWaterTile(int currentVertex,
    //                       glm::vec2 vertexPositions[4],
    //                       int vertex1,
    //                       int vertex2);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWaterTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight);
    //    void storeGridSquare(float col, float row);
    void makeWater();

    //    std::vector<float> m_vertexData;
    std::vector<float> m_vertexData;
    int m_rowCount;
    int m_colCount;
    float m_height;
    float m_wavetime;
    std::vector<float> m_timeOffsets;
};
