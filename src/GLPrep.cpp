//#include "realtime.h"
//#include "realtime.cpp"
//#include "shapes/Sphere.h"
//#include "shapes/Cube.h"
//#include "shapes/Cylinder.h"
//#include "shapes/Cone.h"

//float m_ka = sceneData.globalData.ka;
//float m_kd = sceneData.globalData.kd;
//float m_ks = sceneData.globalData.ks;
//std::vector<std::vector<float>> vertsList;
//std::vector<GLuint> vaos;

//void GLPrep::getVaos() {

//    std::vector<GLuint> vbos;

//    Cube cube{};
//    cube.updateParams(settings.shapeParameter1);
//    std::vector<float> cubeVerts = cube.generateShape();

//    Cone cone{};
//    cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
//    std::vector<float> coneVerts = cone.generateShape();

//    Cylinder cylinder{};
//    cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
//    std::vector<float> cylinderVerts = cube.generateShape();

//    Sphere sphere{};
//    sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
//    std::vector<float> sphereVerts = cube.generateShape();

//    vertsList = {cubeVerts, coneVerts, cylinderVerts, sphereVerts};

//    for (int i = 0; i < 4; i++) {

//        glGenBuffers(1, &vbos[i]);
//        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertsList[i].size(), cubeVerts.data(), GL_STATIC_DRAW);
//        glGenVertexArrays(1, &vaos[i]);
//        glBindVertexArray(vaos[i]);
//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(0, vertsList[i].size()/2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), nullptr);
//        glVertexAttribPointer(1, vertsList[i].size()/2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
//        glBindVertexArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER,0);
//    }
//}

////void GLPrep::draw(RenderShapeData shape, GLuint m_shader) {
////    float m_shininess = shape.primitive.material.shininess;
////    PrimitiveType type = shape.primitive.type;
////    GLuint vao;
////    std::vector<float> verts;
////    if (type == PrimitiveType::PRIMITIVE_CUBE) {
////        vao = vaos[0];
////        verts = vertsList[0];
////    }
////    else if (type == PrimitiveType::PRIMITIVE_CONE) {
////        vao = vaos[1];
////        verts = vertsList[1];
////    }
////    else if (type == PrimitiveType::PRIMITIVE_CYLINDER){
////        vao = vaos[2];
////        verts = vertsList[2];
////    }
////    else {
////        vao = vaos[3];
////        verts = vertsList[3];
////    }

////    glBindVertexArray(vao);
////    glUseProgram(m_shader);

////    GLint uniformLocation = glGetUniformLocation(m_shader, "modelmatrix");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &shape.ctm[0][0]);


////    uniformLocation = glGetUniformLocation(m_shader, "viewmatrix");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

////    uniformLocation = glGetUniformLocation(m_shader, "projmatrix");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.projMatrix()[0][0]);


////    uniformLocation = glGetUniformLocation(m_shader, "ka");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m_ka);

////    uniformLocation = glGetUniformLocation(m_shader, "kd");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m_kd);


////    uniformLocation = glGetUniformLocation(m_shader, "ks");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m_ks);


////    uniformLocation = glGetUniformLocation(m_shader, "shininess");
////    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m_shininess);

////    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 6);

////    glBindVertexArray(0);
////    glUseProgram(0);
////}
