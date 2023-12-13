#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <fstream>
#include <sstream>
#include "camera/camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "settings.h"
#include "GLPrep.h"
#include "shaderloader.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"

// Lights, Camera

// Global variables

SceneMaterial bronzeBall {
    .cAmbient =  SceneColor(glm::vec4(0.3, 0.25, 0.08, 1)),  // Ambient term
    .cDiffuse = SceneColor(glm::vec4(1, 0.5, 0.15, 1)),  // Diffuse term
    .cSpecular = SceneColor(glm::vec4(1, 1, 1, 1)), // Specular term
    .shininess = 60,      // Specular exponent
};

SceneMaterial coal {
    .cAmbient =  SceneColor(glm::vec4(0.05, 0.05, 0.05, 1)),  // Ambient term
    .cDiffuse = SceneColor(glm::vec4(0, 0., 0., 1)),  // Diffuse term
    .cSpecular = SceneColor(glm::vec4(0.1, 0.1, 0.1, 1.0)), // Specular term
    .shininess = 0,      // Specular exponent
};

SceneMaterial redRitchie {
    .cAmbient =  SceneColor(glm::vec4(1.0, 0.0, 0.0, 1.0)),  // Ambient term
    .cDiffuse = SceneColor(glm::vec4(1.0, 0., 0., 1)),  // Diffuse term
    .cSpecular = SceneColor(glm::vec4(1, 1, 1, 1)), // Specular term
    .shininess = 60,      // Specular exponent
};

SceneMaterial smoke{
    .cAmbient = SceneColor(glm::vec4(0.5f, 0.5f, 0.5f, 0.1f)), // Ambient term
    .cDiffuse = SceneColor(glm::vec4(1.0, 1.0, 1.0, 0.1f)), // Diffuse term
    .cSpecular = SceneColor(glm::vec4(0.0, 0.0, 0.0, 0.1f)), // Specular term
    .shininess = 1,       // Specular exponent
};

SceneMaterial redSmoke{
    .cAmbient = SceneColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.1f)), // Ambient term
    .cDiffuse = SceneColor(glm::vec4(1.0, 1.0, 1.0, 0.1f)), // Diffuse term
    .cSpecular = SceneColor(glm::vec4(0.0, 0.0, 0.0, 0.1f)), // Specular term
    .shininess = 1,       // Specular exponent
};
std::vector<SceneMaterial> materialList = {bronzeBall, coal, redRitchie};

ScenePrimitive smokePrimitive{
    .type = PrimitiveType::PRIMITIVE_CUBE,
    .material = smoke,
    .isFire = false,
    .isSmoke = true,
    .isRedSmoke = false,
};
ScenePrimitive redSmokePrimitive{
    .type = PrimitiveType::PRIMITIVE_CUBE,
    .material = redSmoke,
    .isFire = false,
    .isSmoke = false,
    .isRedSmoke = true,
};
RenderData sceneData;
std::vector<RenderShapeData> smokeShapes;
Camera camera(sceneData.cameraData, 0, 0);
Ball ball(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0)), 0.5, bronzeBall);
std::vector<GLuint> vaos(5);
std::vector<GLuint> vbos(5);
float m_accumulatedTime;

// Object data
std::unordered_map<std::string, OBJMaterial> objData;
std::set<std::string> objNames; // keys for accessing the map of objData
std::vector<GLuint> objVaos(999); // objVao
std::vector<GLuint> objVbos(999); // objVbo


Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;
    m_keyMap[Qt::Key_Shift]   = false;
    m_keyMap[Qt::Key_J]       = false;
    m_keyMap[Qt::Key_F]       = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteProgram(m_texture_shader);
    glDeleteProgram(m_shader);
    for (int i = 0; i < 4; i++) {
        glDeleteVertexArrays(1, &vaos[i]);
        glDeleteBuffers(1, &vbos[i]);
    }
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    glDeleteBuffers(1, &m_water_vbo);
    glDeleteVertexArrays(1, &m_water_vao);

    glDeleteTextures(1, &m_kitten_texture); // !!!
    glDeleteProgram(m_object_shader); // ??????????
    for (int j = 0; j < objVaos.size(); j++) {
        glDeleteVertexArrays(1, &objVaos[j]);
        glDeleteBuffers(1, &objVbos[j]);
    }

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_SRC_ALPHA);
    glEnable(GL_ONE_MINUS_SRC_ALPHA);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    extractInfo(settings.sceneFilePath);
    glClearColor(0.12, 0.588, 0.9, 1);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    // ????????????????????????????????????????
    m_object_shader = ShaderLoader::createShaderProgram(":/resources/shaders/object.vert", ":/resources/shaders/object.frag");
    objData.clear();
    objNames.clear();
    objData = objparser.parseMtlFile(":/objects/greens.mtl"); // parse Mtl file and create objData
    objNames = objparser.loadMesh(":/objects/greens.obj", objData); // load the mesh vertex data into corresponding objects
    getObjVaos();
    // ?????????????????????????? //

    glGenBuffers(1, &m_water_vbo);
    glGenVertexArrays(1, &m_water_vao);
    m_accumulatedTime = 0.f;
    Realtime::getVaos();
    updateWater(false);
    Realtime::getFullScreenVao();
    Realtime::makeFBO();

    initialized = true;
    m_fire_center = glm::vec3(2.1, 0.1, 1.2);
}

void Realtime::populateHitObjs() {
    hitObject rock = {
        .position = {0, 10},
        .radius = 0.6,
        .type = HitObjType::HitObj_Sphere
    };

    hitObject tree = {
        .position = {10, 0},
        .radius = 0.3,
        .type = HitObjType::HitObj_Cylinder
    };
    hitObjs.push_back(rock);
    hitObjs.push_back(tree);
}

void Realtime::readTexture() {
    // Prepare filepath
    QString kitten_filepath;
    if (settings.material == 3)  kitten_filepath = QString(":/textures/kitten.png");
    else if (settings.material == 2)  kitten_filepath = QString(":/textures/wood_texture.png");
    QString angry_filepath = QString(":/textures/angry_kitten.png");

    m_image = QImage(kitten_filepath);
    m_angry_image = QImage(angry_filepath);

    m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    m_angry_image = m_angry_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

    glGenTextures(1, &m_kitten_texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_kitten_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(0, m_kitten_texture);

    glUseProgram(m_shader);
    int textureLocation = glGetUniformLocation(m_shader, "sampleTexture");
    glUniform1f(textureLocation, GL_TEXTURE0);  // Assuming you want to use texture slot 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    glUseProgram(0);  // Return to default state
}

// polulates the global variables by parsing the json file
void Realtime::extractInfo(std::string filepath) {
    SceneParser::parse(settings.sceneFilePath, sceneData);
    m_ka = sceneData.globalData.ka;
    m_kd = sceneData.globalData.kd;
    m_ks = sceneData.globalData.ks;
    lightTypes.clear();
    lightDirs.clear();
    lightColors.clear();
    lightPoses.clear();
    functions.clear();
    angles.clear();
    penumbras.clear();
    isFires.clear();
    for (auto light : sceneData.lights) {
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            lightTypes.push_back(0);
            isFires.push_back(light.isFire);
            lightDirs.push_back(light.dir);
            lightColors.push_back(light.color);
            lightPoses.push_back(glm::vec4(999, 999, 999, 999));
            functions.push_back(glm::vec3(1.0f, 0.f, 0.f));
            angles.push_back(0.f);
            penumbras.push_back(0.f);
        } else if (light.type == LightType::LIGHT_POINT) {
            lightTypes.push_back(1);
            isFires.push_back(light.isFire);
            lightDirs.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
            lightColors.push_back(light.color);
            lightPoses.push_back(light.pos);
            functions.push_back(light.function);
            angles.push_back(0.f);
            penumbras.push_back(0.f);
        } else {
            lightTypes.push_back(2);
            isFires.push_back(light.isFire);
            lightDirs.push_back(light.dir);
            lightColors.push_back(light.color);
            lightPoses.push_back(light.pos);
            functions.push_back(light.function);
            angles.push_back(light.angle);
            penumbras.push_back(light.penumbra);
        }
    }
    int c = 0;
    m_fire_pos = glm::vec3(0.0);
    for (auto shape : sceneData.shapes) {
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_WATER) {
            glm::mat4 ctm = shape.ctm;
            m_topLeft = ctm * glm::vec4(-1, 0.0, 1, 1.0);
            m_topRight = ctm * glm::vec4(1, 0.0, 1, 1.0);
            m_bottomRight = ctm * glm::vec4(1, 0.0, -1, 1.0);
            m_bottomLeft = ctm * glm::vec4(-1, 0.0, -1, 1.0);
        }
        else if (shape.isFire && shape.primitive.type != PrimitiveType::PRIMITIVE_CUBE) {
            m_fire_pos += glm::vec3(shape.ctm * glm::vec4(0.0, 0.0, 0.0, 1.0));
            c += 1;
        }
    }
    m_fire_pos = m_fire_pos * (1.0f/float(c));

}

void Realtime::updateWater(bool initialized) {

    // ================= PrimitiveType::PRIMITIVE_WATER ================= //
    water.updateParams(20, 20, 0.f, m_accumulatedTime, initialized);
    m_waterData = water.generateShape();

    glBindBuffer(GL_ARRAY_BUFFER, m_water_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_waterData.size() * sizeof(float), m_waterData.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_water_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::getFullScreenVao() {

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(m_texture_shader);
    GLint uniformLocation = glGetUniformLocation(m_texture_shader, "sampler");
    glUniform1f(uniformLocation, GL_TEXTURE0);
    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        {
            -1.0f,  1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,

            -1.0f, -1.0f, 0.0f,
            0.0f,  0.0f, 0.0f,

            1.0f, -1.0f, 0.0f,
            1.0f,  0.0f, 0.0f,

            1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,

            1.0f, -1.0f, 0.0f,
            1.0f,  0.0f, 0.0f,
        };

    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Realtime::makeFBO(){

    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

}

void Realtime::getVaos() {

    Cube cube{};
    cube.updateParams(1);
    std::vector<float> cubeVerts = cube.generateShape();

    Cone cone{};
    cone.updateParams(5, 5);
    std::vector<float> coneVerts = cone.generateShape();

    Cylinder cylinder{};
    cylinder.updateParams(5, 5);
    std::vector<float> cylinderVerts = cylinder.generateShape();

    Sphere sphere{};
    sphere.updateParams(5, 5, 0.5);
    std::vector<float> sphereVerts = sphere.generateShape();

    Sphere ballShape{};
    ballShape.updateParams(7, 7, ball.getRadius());
    std::vector<float> ballVerts = ballShape.generateShape();

    vertsList = {cubeVerts, coneVerts, cylinderVerts, sphereVerts, ballVerts};

    for (int i = 0; i < 5; i++) {
        glDeleteVertexArrays(1, &vaos[i]);
        glDeleteBuffers(1, &vbos[i]);
        glGenBuffers(1, &vbos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertsList[i].size(), vertsList[i].data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Realtime::getObjVaos() { // !!!!!!!!!!!!!!!!!!!!!??????????????
    int i = 50;
    //    std::vector<GLuint> objVaos;
    //    std::vector<GLuint> objVbos;
    for (auto it = objNames.begin(); it != objNames.end(); ++it, ++i) {
        // '(*it)' is the current string in the set
        glDeleteVertexArrays(1, &objVaos[i]); // ??
        glDeleteBuffers(1, &objVbos[i]); // ??
        glGenBuffers(1, &objVbos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, objVbos[i]);
        std::vector<float> obj_vertexData = objData[*it].obj_vertexData;
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_vertexData.size(), obj_vertexData.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &objVaos[i]);
        glBindVertexArray(objVaos[i]);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Realtime::paintObj() {

    glm::vec4 cameraPos = camera.getData().pos;

    glm::mat4 identityMatrix(1.0f);
    glUseProgram(m_object_shader);

    GLint uniformLocation = glGetUniformLocation(m_object_shader, "viewmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    uniformLocation = glGetUniformLocation(m_object_shader, "projmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
    uniformLocation = glGetUniformLocation(m_object_shader, "cameraPos");
    glUniform4f(uniformLocation, cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[3]);

    int numLights = sceneData.lights.size();
    uniformLocation = glGetUniformLocation(m_object_shader, "numLights");
    glUniform1i(uniformLocation, numLights);

    for (int i = 0; i < sceneData.lights.size(); i++) {
        std::string str =  "lightTypes[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform1i(uniformLocation, lightTypes[i]);

        str =  "lightPoses[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform4f(uniformLocation, lightPoses[i][0],  lightPoses[i][1],  lightPoses[i][2],  lightPoses[i][3]);

        str =  "lightColors[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform4f(uniformLocation, lightColors[i][0], lightColors[i][1], lightColors[i][2], lightColors[i][3]);

        str =  "lightDirs[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform4f(uniformLocation, lightDirs[i][0], lightDirs[i][1], lightDirs[i][2], lightDirs[i][3]);

        str =  "functions[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform3f(uniformLocation, functions[i][0], functions[i][1], functions[i][2]);

        str =  "angles[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform1f(uniformLocation, angles[i]);

        str =  "penumbras[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_object_shader, str.c_str());
        glUniform1f(uniformLocation, penumbras[i]);
    }

    int i = 50;
    for (auto it = objNames.begin(); it != objNames.end(); ++it, ++i) {
        uniformLocation = glGetUniformLocation(m_object_shader, "cAmbient");
        glUniform4fv(uniformLocation, 1, objData[*it].ambient);

        uniformLocation = glGetUniformLocation(m_object_shader, "cDiffuse");
        glUniform4fv(uniformLocation, 1, objData[*it].diffuse);

        uniformLocation = glGetUniformLocation(m_object_shader, "cSpecular");
        glUniform4fv(uniformLocation, 1, objData[*it].specular);

        uniformLocation = glGetUniformLocation(m_object_shader, "shininess");
        glUniform1f(uniformLocation, objData[*it].shininess);

        glBindVertexArray(objVaos[i]);
        glDrawArrays(GL_TRIANGLES, 0, objData[*it].obj_vertexData.size() / 6);
        glBindVertexArray(0);
    }
    glUseProgram(0);
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void Realtime::draw(RenderShapeData& shape, bool ifBall, glm::mat4 originalCTM) {

    glm::vec4 cameraPos = camera.getData().pos;
    int numLights = sceneData.lights.size();

    PrimitiveType type = shape.primitive.type;
    GLuint vao = 0;
    std::vector<float> verts;
    glm::mat4 ctm = shape.ctm;

    glm::vec4 cAmbient = shape.primitive.material.cAmbient;
    glm::vec4 cDiffuse = shape.primitive.material.cDiffuse;
    glm::vec4 cSpecular = shape.primitive.material.cSpecular;
    float shininess = shape.primitive.material.shininess;
    glm::vec3 fire_rise = glm::vec3(0.001, 0.005, 0.001);

    glUseProgram(m_shader);
    GLint primTypeLocation = glGetUniformLocation(m_shader, "primitive");

    if (ifBall) {
        vao = vaos[4];
        verts = vertsList[4];
        cAmbient = ball.getMaterial().cAmbient;
        cDiffuse = ball.getMaterial().cDiffuse;
        cSpecular = ball.getMaterial().cSpecular;
        shininess = ball.getMaterial().shininess;
        shape.ctm = ball.getCTM();

        glUniform1i(primTypeLocation, 3);
    }

    else if (type == PrimitiveType::PRIMITIVE_CUBE) {
        if (shape.isSmoke || shape.isRedSmoke){
            float x_dist = shape.ctm[3][0] - ball.getPos().x;
            float z_dist = shape.ctm[3][2] - ball.getPos().z;
            glm::vec2 xz_vel = glm::vec2(glm::normalize(glm::vec2(x_dist, z_dist))[0], glm::normalize(glm::vec2(x_dist, z_dist))[1]);
            float y_vel = 0.2f;
            glm::vec3 velocity;
            if (shape.riseCount == 0){
                shape.timeOffset = ((double) rand() / (RAND_MAX)) * 60.f;
                shape.ctm = shape.ctm * glm::translate(glm::mat4(1.f), glm::vec3(0, shape.timeOffset * y_vel, 0));
            }
            shape.riseCount += 1;
            float r = ((double) rand() / (RAND_MAX)) * 0.4f;
            if (shape.riseCount <= 8){
                velocity = glm::vec3(r * xz_vel[0], (double) rand() / (RAND_MAX) * y_vel, r * xz_vel[1]);
            }else if(shape.riseCount <= 10){
                velocity = glm::vec3(0.f, (double) rand() / (RAND_MAX) * y_vel, 0.f);
            }else if (shape.riseCount <= 18){
                velocity = glm::vec3(-r * xz_vel[0], (double) rand() / (RAND_MAX) * y_vel, -r * xz_vel[1]);
            }
            if (shape.riseCount + shape.timeOffset > 44.f){
                return;
            }else{
                shape.ctm = shape.ctm * glm::translate(glm::mat4(1.f), velocity);
                if (shape.isRedSmoke){
                    shape.primitive.material.cAmbient[1] += 0.021f;
                }
            }
        }
        else if (shape.isFire){
            if (fireOn || ctm != originalCTM){
                float x_dist = shape.ctm[3][0] - m_fire_center.x;
                float z_dist = shape.ctm[3][2] - m_fire_center.z;
                glm::vec2 xz_vel = glm::vec2(-glm::normalize(glm::vec2(x_dist, z_dist))[1], glm::normalize(glm::vec2(x_dist, z_dist))[0]);
                float y_vel = 0.1f;
                glm::vec3 velocity;
                if (shape.riseCount == 0){
                    shape.timeOffset = ((double) rand() / (RAND_MAX)) * 60.f;
                    shape.ctm = shape.ctm * glm::translate(glm::mat4(1.f), glm::vec3(0, shape.timeOffset * y_vel, 0));
                }
                shape.riseCount += 1;
                float r = ((double) rand() / (RAND_MAX)) * 0.04f;
                if (shape.riseCount <= 30){
                    velocity = glm::vec3(r * xz_vel[0], (double) rand() / (RAND_MAX) * y_vel, r * xz_vel[1]);
                }else if(shape.riseCount <= 80){
                    velocity = glm::vec3(0.f, (double) rand() / (RAND_MAX) * y_vel, 0.f);
                }else{
                    velocity = glm::vec3(-r * xz_vel[0], (double) rand() / (RAND_MAX) * y_vel, -r * xz_vel[1]);
                }
                if (shape.riseCount + shape.timeOffset >= 99.f){
                    shape.ctm = originalCTM;
                    shape.primitive.material.cAmbient[1] = 0.0f;
                    shape.riseCount = 0;
                }else{
                    shape.ctm = shape.ctm * glm::translate(glm::mat4(1.f), velocity);
                    shape.primitive.material.cAmbient[1] += 0.007f;
                }
            } else {
                return;
            }
        }
        vao = vaos[0];
        verts = vertsList[0];
        glUniform1i(primTypeLocation, 0);
    }
    else if (type == PrimitiveType::PRIMITIVE_CONE) {
        vao = vaos[1];
        verts = vertsList[1];
        glUniform1i(primTypeLocation, 1);
    }
    else if (type == PrimitiveType::PRIMITIVE_CYLINDER){
        vao = vaos[2];
        verts = vertsList[2];
        glUniform1i(primTypeLocation, 2);
    }
    else if (type == PrimitiveType::PRIMITIVE_SPHERE){
        vao = vaos[3];
        verts = vertsList[3];
        glUniform1i(primTypeLocation, 3);
    }
    else if (type == PrimitiveType::PRIMITIVE_WATER){
        vao = m_water_vao;
        verts = m_waterData;
    }

    glBindVertexArray(vao);

    GLint uniformLocation = glGetUniformLocation(m_shader, "modelmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &shape.ctm[0][0]);

    uniformLocation = glGetUniformLocation(m_shader, "viewmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

    uniformLocation = glGetUniformLocation(m_shader, "projmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);

    cAmbient *= m_ka;
    cDiffuse *= m_kd;
    cSpecular *= m_ks;

    uniformLocation = glGetUniformLocation(m_shader, "cAmbient");
    if (ifBall && settings.material == 1) cAmbient = fmin(1.0f, (1.0f - time_on_fire/10.0f)) * cAmbient + fmax(0.0f, time_on_fire/10.0f) * glm::vec4(1.0, 0.25, 0.23, 1.0);
    glUniform4f(uniformLocation, cAmbient[0], cAmbient[1], cAmbient[2], cAmbient[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cDiffuse");
    if (ifBall && settings.material == 1) cDiffuse = fmin(1.0f, (1.0f - time_on_fire/10.0f)) * cDiffuse + fmax(0.0f, time_on_fire/10.0f) * glm::vec4(1.0, 0.0, 0.25, 1.0);
    glUniform4f(uniformLocation, cDiffuse[0], cDiffuse[1], cDiffuse[2], cDiffuse[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cSpecular");
    glUniform4f(uniformLocation, cSpecular[0], cSpecular[1], cSpecular[2], cSpecular[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cameraPos");
    glUniform4f(uniformLocation, cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[3]);

    uniformLocation = glGetUniformLocation(m_shader, "shininess");
    glUniform1f(uniformLocation, shininess);

    uniformLocation = glGetUniformLocation(m_shader, "numLights");
    glUniform1i(uniformLocation, numLights);

    uniformLocation = glGetUniformLocation(m_shader, "blend");
    float blend = 0.0f;
    if (ifBall && settings.material == 2) blend = fmin(1.0f, (1.0f - time_on_fire/10.0f));
    else if (ifBall && settings.material == 3) blend = fmin(1.0f, (1.0f - 0.6 * time_on_fire/10.0f));
    glUniform1f(uniformLocation, blend);

    uniformLocation = glGetUniformLocation(m_shader, "anger");
    float anger = 0.0f;
    if (ifBall && settings.material != 1) anger = fmax(0.0f, time_on_fire/10.0f);
    glUniform1f(uniformLocation, anger);

    glUniform1i(glGetUniformLocation(m_shader, "fireOn"), fireOn);
    for (int i = 0; i < sceneData.lights.size(); i++) {

        std::string str =  "lightTypes[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform1i(uniformLocation, lightTypes[i]);

        str = "isFires[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform1i(uniformLocation, isFires[i]);

        str =  "lightPoses[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform4f(uniformLocation, lightPoses[i][0],  lightPoses[i][1],  lightPoses[i][2],  lightPoses[i][3]);

        str =  "lightColors[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform4f(uniformLocation, lightColors[i][0], lightColors[i][1], lightColors[i][2], lightColors[i][3]);

        str =  "lightDirs[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform4f(uniformLocation, lightDirs[i][0], lightDirs[i][1], lightDirs[i][2], lightDirs[i][3]);

        str =  "functions[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform3f(uniformLocation, functions[i][0], functions[i][1], functions[i][2]);

        str =  "angles[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform1f(uniformLocation, angles[i]);

        str =  "penumbras[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform1f(uniformLocation, penumbras[i]);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_kitten_texture);

    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 6);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintGL() {
    //  Students: anything requiring OpenGL calls every frame should be done here
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width*  m_devicePixelRatio, m_height * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (sceneLoaded) {
        RenderShapeData ball = sceneData.shapes[0];
        Realtime::draw(ball, true, ball.ctm);  // draw the ball
    }
    if (time_on_fire > 1.f) {
        glm::vec3 ballPos = ball.getPos();
        for (int i = -1; i < 2 ; i ++){
            for (int j = 1; j < 2; j ++){
                for (int k = -1; k < 2; k ++){
                    glm::mat4 ctm = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(ballPos.x + float(i) * 0.2f, ballPos.y + float(j) * 0.2f, ballPos.z + float(k) * 0.2f)), glm::vec3(0.15f, 0.15f, 0.15f));
                    bool bronzeSteam = (settings.material == 1) && (isInWater());
                    bool coalBurn = (settings.material == 2) && (time_on_fire < 8) && (!isInWater());
                    bool woodSteam = (settings.material == 2) && (! ((time_on_fire < 8) && (!isInWater())));
                    bool DRSteam = settings.material == 3;
                    if (bronzeSteam || woodSteam || DRSteam){
                        RenderShapeData smoke = RenderShapeData{
                            .primitive = smokePrimitive,
                            .ctm = ctm,
                            .originalCTM = ctm,
                            .isFire = false,
                            .isSmoke = true,
                            .isRedSmoke = false,
                            .riseCount = 0,
                            .timeOffset = 0,
                        };
                        smokeShapes.push_back(smoke);
                    }else if(coalBurn){
                        RenderShapeData smoke = RenderShapeData{
                            .primitive = redSmokePrimitive,
                            .ctm = ctm,
                            .originalCTM = ctm,
                            .isFire = false,
                            .isSmoke = false,
                            .isRedSmoke = true,
                            .riseCount = 0,
                            .timeOffset = 0,
                        };
                        smokeShapes.push_back(smoke);
                    }
                }
            }
        }
    } else if (time_on_fire <= -2.9f) {
        smokeShapes.clear();
    }
    for (RenderShapeData &shape : sceneData.shapes) {
        Realtime::draw(shape, false, shape.originalCTM);
    }
    for (RenderShapeData &shape : smokeShapes){
        Realtime::draw(shape, false, shape.originalCTM);
    }
    paintObj();

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture);
}

void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_texture_shader);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_width = w;
    m_height = h;
    camera.updateWH(m_width, m_height);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    makeFBO();
}

void Realtime::sceneChanged() {
    sceneLoaded = true;
    update(); // asks for a PaintGL() call to occur
    extractInfo(settings.sceneFilePath);
    ball = Ball(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0)), 0.5, m_ballMaterial);
    SceneCameraData cData = sceneData.cameraData;
    cData.look = ball.getPos() - cData.pos;
    camera = Camera(cData, m_width, m_height);
    fireOn = true;
    soaked = false;
    time_on_fire = -3.0f;
    onRock = false;
}

void Realtime::settingsChanged() {
    m_ballMaterial = materialList[settings.material-1];
    if (initialized) {
        readTexture();
        update(); // asks for a PaintGL() call to occur
    }
    ball.changeMaterial(materialList[settings.material-1]);
    if (settings.material == 1) soaked = false;
    populateHitObjs();
}

// ================== Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}


void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        if (!m_mouseDown) return;

        SceneCameraData cData = camera.getData();

        float theta_x = M_PI * deltaX / m_width;
        float theta_y = M_PI * deltaY / m_height;

        glm::vec3 pos = glm::vec3(cData.pos);
        glm::vec3 up = glm::vec3(glm::normalize(cData.up));
        glm::vec3 look = glm::vec3(cData.look);
        glm::vec3 left = glm::normalize(glm::cross(up, look));

        glm::mat4 rx = glm::rotate(glm::mat4(1.0), -theta_x, glm::vec3(0.f, 1.0f, 0.f));
        glm::mat4 ry = glm::rotate(glm::mat4(1.0), theta_y, left);
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), -glm::vec3(ball.getPos()));
        glm::mat4 transBack = glm::translate(glm::mat4(1.0f), glm::vec3(ball.getPos()));
        glm::vec4 camPosBall = trans * cData.pos;
        camPosBall = rx * ry * camPosBall;
        cData.pos = transBack * camPosBall;
        cData.look = ball.getPos() - cData.pos;
        camera.updateData(cData);

        update(); // asks for a PaintGL() call to occur
    }
}

bool Realtime::isInWater() {
    glm::vec4 ballPos = ball.getPos();

    return ballPos.x >= m_topLeft.x && ballPos.z <= m_topLeft.z && ballPos.x <= m_bottomRight.x && ballPos.z >= m_bottomRight.z;
}

glm::vec3 Realtime::getWaterNormal() {

    glm::vec3 ballPos = ball.getPos();

    glm::vec3 n = {0.0, 0.0, 0.0};

    if (ballPos.x <= m_topLeft.x + m_rim_width) {
        n += glm::vec3{1, 1, 0};
    }
    if (ballPos.z >= m_topLeft.z - m_rim_width) {
        n += glm::vec3{0, 1, -1};
    }
    if (ballPos.x >= m_bottomRight.x - m_rim_width) {
        n += glm::vec3{-1, 1, 0};
    }
    if (ballPos.z <= m_bottomRight.z + m_rim_width) {
        n += glm::vec3{0, 1, 1};
    }
    if (n == glm::vec3{0.0, 0.0, 0.0}) return glm::vec3(0.0, 1.0, 0.0);
    return glm::normalize(n);
}

// get the moving direction
glm::vec3 Realtime::getDir(bool w, bool s, bool a, bool d) {

    SceneCameraData cData = camera.getData();
    glm::vec3 look = glm::vec3(glm::normalize(cData.look));
    glm::vec3 up = glm::vec3(glm::normalize(cData.up));
    glm::vec3 left = glm::normalize(glm::cross(up, look));
    glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 forward = look - glm::dot(look, n) * n;
    glm::vec3 ballPos = glm::vec3(ball.getPos());
    if (ballPos.x > m_bound || ballPos.z > m_bound || ballPos.x < -m_bound || ballPos.z < -m_bound) forward = look; // rocket!
    left = left - glm::dot(left, n) * n;
    glm::vec3 desiredDir = glm::vec3(0.0f);

    if (w) {
        desiredDir += forward;
    }
    if (s) {
        desiredDir += -forward;
    }
    if (a) {
        desiredDir += left;
    }
    if (d) {
        desiredDir += -left;
    }

    if (desiredDir == glm::vec3(0.0)) return glm::vec3(0.0f);

    desiredDir = glm::normalize(desiredDir);

    if (isInWater()) {
        n = getWaterNormal();
        desiredDir = desiredDir - glm::dot(desiredDir, n) * n;
        return desiredDir;
    }

    onFire = false;
    onRock = false;


    if (glm::distance(m_fire_pos, ballPos) <= ball.getRadius() + m_fire_radius){
        onFire = true;
        if (glm::distance(m_fire_pos, ballPos) > m_fire_radius) {
            if ((glm::dot(desiredDir, ballPos - m_fire_pos) > 0.0f && ballPos.y > ball.getRadius()) ||
                (glm::dot(desiredDir, ballPos - m_fire_pos) <= 0.0f && ballPos.y < ball.getRadius() + 0.4)) {
                n = ballPos - m_fire_pos;
                n.y = 0;
                n = glm::normalize(glm::vec3(0, 0.7, 0) + glm::normalize(n));
                desiredDir = desiredDir - glm::dot(desiredDir, n) * n;
                return desiredDir;
            }
        }
    }

    for (auto obj : hitObjs) {
        auto type = obj.type;
        glm::vec3 objPos = glm::vec3(obj.position[0], ballPos.y, obj.position[1]);
        float dist = glm::distance(ballPos, objPos);
        if (dist <= ball.getRadius() + obj.radius) {
            if (type == HitObjType::HitObj_Cylinder && (glm::dot(desiredDir, objPos - ballPos) > 0.0f)) {
                n = glm::normalize(glm::vec3(ballPos.x - obj.position[0], 0.0f, ballPos.z - obj.position[1]));
                desiredDir = desiredDir - glm::dot(desiredDir, n) * n;
                return desiredDir;
            } else {
                onRock = true;
                if ((glm::dot(desiredDir, objPos - ballPos) > 0.0f) || ballPos.y > ball.getRadius()) {
                    n = glm::normalize(ballPos - glm::vec3(obj.position[0], 0.0f, obj.position[1]));
                    desiredDir = desiredDir - glm::dot(desiredDir, n) * n;
                    return desiredDir;
                }
            }
        }
    }
    return desiredDir;
}

void Realtime::updateBallAndFireStates(float deltaTime, glm::mat4 &ctm, SceneCameraData &cData) {

    glm::vec3 ballPos = ball.getPos();


    // check if ball is soaked
    if (settings.material != 1 && ballPos.y <= 0.1) {
        soaked = true;
    }

    // check if ball is on fire and if fire is on to update time_on_fire
    if (onFire && soaked && fireOn) {
        // if ball is on the fireplace, it is soaked, and the fire is on, put out the fire, and dry the ball.
        fireOn = false;
        soaked = false;
    }
    if (onFire && fireOn) {
        time_on_fire = fmin(10.0f, time_on_fire + deltaTime);
    }
    else if (ballPos.y <= ball.getRadius() - 0.2) {
        time_on_fire = fmax(-3.0f, time_on_fire - deltaTime * 5);
    }
    else if (settings.material != 2) {
        time_on_fire = fmax(-3.0f, time_on_fire - deltaTime);
    }
    if (m_keyMap[Qt::Key_F]) fireOn = true;  // press F to light on fire

    // jump or drop down to the ground
    float groundHeight = ball.getRadius();
    if (onFire) groundHeight += 0.5;
    if (onRock) groundHeight += 0.5;
    if (m_keyMap[Qt::Key_J] && ballPos.y <= groundHeight + 0.5) {
        ctm = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.1, 0.0)) * ctm;
    }
    if (ballPos.x < m_bound && ballPos.z < m_bound &&
        ballPos.x > -m_bound && ballPos.z > -m_bound &&
        ballPos.y > groundHeight && !m_keyMap[Qt::Key_J]) {
        ctm = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.08, 0.0)) * ctm;
    }
}


void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    m_accumulatedTime += elapsedms * 0.0005f; // adjust

    updateWater(true);

    SceneCameraData cData = camera.getData();
    glm::mat4 ctm = ball.getCTM();
    glm::vec3 look = glm::vec3(glm::normalize(cData.look));
    glm::vec3 up = glm::vec3(glm::normalize(cData.up));
    float speed = 4.0;

    if (settings.material == 1) speed = 3.0;
    else if (settings.material == 2) speed = 6.0;

    if (m_keyMap[Qt::Key_Shift]) speed *= 2.5;

    if (isInWater() && settings.material == 1) speed *= 0.8;
    else if (isInWater() && settings.material == 2) speed *= 0.5;
    else if (isInWater()) speed *= 0.6;
    float dist = speed * deltaTime;

    glm::vec3 dir = getDir(m_keyMap[Qt::Key_W], m_keyMap[Qt::Key_S], m_keyMap[Qt::Key_A], m_keyMap[Qt::Key_D]);
    glm::vec3 rotAxis = glm::normalize(glm::cross(up, dir));

    if (m_keyMap[Qt::Key_Control]) {
        if (glm::length(ball.getPos()-cData.pos) >= 0.2) {
            glm::mat4 trans = glm::translate(glm::mat4(1.0), look * dist * 3.0f);
            cData.pos = trans * cData.pos;
        }
    }

    if (m_keyMap[Qt::Key_Space]) {
        glm::mat4 trans = glm::translate(glm::mat4(1.0), -look * dist * 3.0f);
        cData.pos = trans * cData.pos;
    }

    if (dir != glm::vec3(0.0) && rotAxis != glm::vec3(0.0)) {
        rotAxis = glm::normalize(rotAxis);
        glm::mat4 trans = glm::translate(glm::mat4(1.0), dir * dist);
        float theta = glm::length(dir * dist) / ball.getRadius();
        cData.pos = trans * cData.pos;
        ctm = trans * ctm;
        glm::vec3 realPos = glm::vec3(ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.00f));
        glm::mat4 rot = glm::translate(glm::rotate(glm::translate(glm::mat4(1.0), realPos), theta, rotAxis), -realPos);
        ctm = rot * ctm;
    }

    glm::vec3 ballPos = ball.getPos();

    updateBallAndFireStates(deltaTime, ctm, cData);

    ball.updateCTM(ctm);
    camera.updateData(cData);
    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
