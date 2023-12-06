#pragma once

#include <vector>
#include <glm/glm.hpp>

class Triangle
{
public:
    void updateParams();
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
};
