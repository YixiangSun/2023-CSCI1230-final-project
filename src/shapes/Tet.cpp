#include "Tet.h"

void Tetrahedron::setVertexData() {
    // Task 8: Find and correct the bug in this method so that the tetrahedron is
    //         rendered with proper shading on all faces

    // define vertices of tetrahedron
    glm::vec3 v_a(0.5, 0.5, 0.5);
    glm::vec3 v_b(-0.5, 0.5, -0.5);
    glm::vec3 v_c(-0.5, -0.5, 0.5);
    glm::vec3 v_d(0.5, -0.5, -0.5);

    // define face normals to be assigned to vertices
    /// 1) triangle ABC
    glm::vec3 normal_abc = glm::cross( (v_b-v_a), (v_c-v_a) );
    /// 2) triangle ACD
    glm::vec3 normal_acd = glm::cross( (v_c-v_a), (v_d-v_a) );
    /// 3) triangle ADB
    glm::vec3 normal_adb = glm::cross( (v_d-v_a), (v_b-v_a) );
    /// 4) triangle BCD
    glm::vec3 normal_bdc = glm::cross( (v_d-v_b), (v_c-v_b) );

    // normalize normals to unit length
    normal_abc = glm::normalize(normal_abc);
    normal_acd = glm::normalize(normal_acd);
    normal_adb = glm::normalize(normal_adb);
    normal_bdc = glm::normalize(normal_bdc);



    //////////////////////////////////////////////////////////////
    //// YOU DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE  /////
    //////////////////////////////////////////////////////////////

    // insert triangle vertex & normal data.
    /// 1) triangle ABC
    insertVec3(m_vertexData, v_a);
    insertVec3(m_vertexData, normal_abc);
    insertVec3(m_vertexData, v_b);
    insertVec3(m_vertexData, normal_abc);
    insertVec3(m_vertexData, v_c);
    insertVec3(m_vertexData, normal_abc);

    /// 2) triangle ACD
    insertVec3(m_vertexData, v_a);
    insertVec3(m_vertexData, normal_acd);
    insertVec3(m_vertexData, v_c);
    insertVec3(m_vertexData, normal_acd);
    insertVec3(m_vertexData, v_d);
    insertVec3(m_vertexData, normal_acd);

    /// 3) triangle ADB
    insertVec3(m_vertexData, v_a);
    insertVec3(m_vertexData, normal_adb);
    insertVec3(m_vertexData, v_d);
    insertVec3(m_vertexData, normal_adb);
    insertVec3(m_vertexData, v_b);
    insertVec3(m_vertexData, normal_adb);

    /// 4) triangle BDC
    insertVec3(m_vertexData, v_b);
    insertVec3(m_vertexData, normal_bdc);
    insertVec3(m_vertexData, v_d);
    insertVec3(m_vertexData, normal_bdc);
    insertVec3(m_vertexData, v_c);
    insertVec3(m_vertexData, normal_bdc);
}

// Inserts a glm::vec3 into a vector of floats.
void Tetrahedron::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Tetrahedron::updateParams() {
    m_vertexData = std::vector<float>();
    setVertexData();
}
