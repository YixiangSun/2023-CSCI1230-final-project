#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "ball.h"
#include "camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "settings.h"
#include "GLPrep.h"
#include "shaderloader.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "fire.h"

// ================== Project 5: Lights, Camera

// Global variables

SceneMaterial ballMaterial {
    .cAmbient =  SceneColor(glm::vec4(1, 1, 1, 1)),  // Ambient term
    .cDiffuse = SceneColor(glm::vec4(1, 1, 1, 1)),  // Diffuse term
    .cSpecular = SceneColor(glm::vec4(0, 0, 0, 0)), // Specular term
    .shininess = 0,      // Specular exponent
    .cReflective = SceneColor(glm::vec4(0, 0, 0, 0)), // Used to weight contribution of reflected ray lighting (via multiplication)
    .blend = 0.5
};

RenderData sceneData;
Camera camera(sceneData.cameraData, 0, 0);
Ball ball(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.3, 0)), 0.3, ballMaterial);
std::vector<GLuint> vaos(5);
std::vector<GLuint> vbos(5);


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

    // If you must use this function, do not edit anything above this
}

void Realtime::extractInfo(std::string filepath, RenderData &renderData) {
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
    for (auto light : sceneData.lights) {
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            lightTypes.push_back(0);
            lightDirs.push_back(light.dir);
            lightColors.push_back(light.color);
            lightPoses.push_back(glm::vec4(999, 999, 999, 999));
            functions.push_back(glm::vec3(1.0f, 0.f, 0.f));
            angles.push_back(0.f);
            penumbras.push_back(0.f);
        } else if (light.type == LightType::LIGHT_POINT) {
            lightTypes.push_back(1);
            lightDirs.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
            lightColors.push_back(light.color);
            lightPoses.push_back(light.pos);
            functions.push_back(light.function);
            angles.push_back(0.f);
            penumbras.push_back(0.f);
        } else {
            lightTypes.push_back(2);
            lightDirs.push_back(light.dir);
            lightColors.push_back(light.color);
            lightPoses.push_back(light.pos);
            functions.push_back(light.function);
            angles.push_back(light.angle);
            penumbras.push_back(light.penumbra);
        }
    }
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
    extractInfo(settings.sceneFilePath, sceneData);
    glClearColor(0,0,0,1);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    Realtime::getVaos();
    Realtime::getFullScreenVao();
    Realtime::makeFBO();

    initialized = true;
    // initialize fire system
    m_FireSystem = Fire();
    // initialize fire
    m_Fire.ColorBegin = {1.0f, 0.0f, 0.0f, 1.0f};
    m_Fire.ColorEnd = {0.5f, 0.5f, 0.0f, 1.0f};
    m_Fire.SizeBegin = 0.3f;
    m_Fire.SizeVariation = 0.1f;
    m_Fire.SizeEnd = 0.2f;
    m_Fire.LifeTime = 1.0f;
    m_Fire.Velocity = {0.0f, 0.2f, 0.0f};
    m_Fire.Position = {0.0f, 3.0f, 0.0f};

    m_FireSystem.Emit(m_Fire);

//    int height = 7;
//    int width = 5;
//    for (int h = 0; h < height; h ++){
//        for (int r = 0; r < width; r ++){
//            for (int c = 0; c < width; c ++){
//                m_Fire.Position = {3.0f + r * 0.1f, 0.05f + h * 0.1f, 3.0f + c * 0.1f};
//                m_FireSystem.Emit(m_Fire);
//            }
//        }
//    }
}
void Realtime::drawFire(){
    int width = 5;
    int height = 7;
    SceneMaterial fireMaterial{
        .cAmbient = SceneColor(glm::vec4(1.0, 0.0, 0.0, 1.0)),
        .cDiffuse = SceneColor(glm::vec4(1.0, 0.0, 0.0, 1.0)),
        .cSpecular = SceneColor(glm::vec4(0.5, 0.5, 0.5, 1.0)),
        .shininess = 20.0
    };
    for (int h = 0; h < height; h ++){
        for (int r = 0; r < width - h; r ++){
            for (int c = 0; c < width - h; c ++){
                if (h < 2 && std::abs(r - width/2) < 1 && std::abs(c - width/2) < 1){
                    fireMaterial.cAmbient = SceneColor(glm::vec4(1.0, 200.f/255.f, 0.0, 1.0));
                }else if(h < 3 && std::abs(r - width/2) < 2 && std::abs(c - width/2) < 2){
                    fireMaterial.cAmbient = SceneColor(glm::vec4(1.0, 100.f/255.f, 0.0, 1.0));
                }else{
                    fireMaterial.cAmbient = SceneColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
                }
                RenderShapeData fire_particle;
                glm::mat4 translate = glm::mat4{
                    1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    3.0 + (r + h/2) * 0.3, 0.15 + h * 0.3, 3.0 + (c + h/2) * 0.3, 1.0
                };
                fire_particle.ctm = glm::scale(glm::rotate(translate, 0.9f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(0.2, 0.2, 0.003));
                fire_particle.primitive = ScenePrimitive{
                    .type = PrimitiveType::PRIMITIVE_CUBE,
                    .material = fireMaterial
                };
                sceneData.shapes.push_back(fire_particle);
            }
        }
    }
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

void Realtime::getVaos() {

    Cube cube{};
    cube.updateParams(settings.shapeParameter1);
    std::vector<float> cubeVerts = cube.generateShape();

    Cone cone{};
    cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    std::vector<float> coneVerts = cone.generateShape();

    Cylinder cylinder{};
    cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    std::vector<float> cylinderVerts = cylinder.generateShape();

    Sphere sphere{};
    sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2, 0.5);
    std::vector<float> sphereVerts = sphere.generateShape();

    Sphere ballShape{};
    ballShape.updateParams(settings.shapeParameter1, settings.shapeParameter2, ball.getRadius());
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

void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

}

void Realtime::draw(RenderShapeData shape, bool ifBall) {

    glm::vec4 cameraPos = camera.getData().pos;
    int numLights = sceneData.lights.size();

    PrimitiveType type = shape.primitive.type;
    GLuint vao;
    std::vector<float> verts;
    glm::mat4 ctm = shape.ctm;

    glm::vec4 cAmbient = shape.primitive.material.cAmbient;
    glm::vec4 cDiffuse = shape.primitive.material.cDiffuse;
    glm::vec4 cSpecular = shape.primitive.material.cSpecular;
    float shininess = shape.primitive.material.shininess;

    if (ifBall) {
        vao = vaos[4];
        verts = vertsList[4];
        cAmbient = ballMaterial.cAmbient;
        cDiffuse = ballMaterial.cDiffuse;
        cSpecular = ballMaterial.cSpecular;
        shininess = ballMaterial.shininess;
        ctm = ball.getCTM();
    }

    else if (type == PrimitiveType::PRIMITIVE_CUBE) {
        vao = vaos[0];
        verts = vertsList[0];
    }
    else if (type == PrimitiveType::PRIMITIVE_CONE) {
        vao = vaos[1];
        verts = vertsList[1];
    }
    else if (type == PrimitiveType::PRIMITIVE_CYLINDER){
        vao = vaos[2];
        verts = vertsList[2];
    }
    else {
        vao = vaos[3];
        verts = vertsList[3];
    }

    glBindVertexArray(vao);
    glUseProgram(m_shader);

    GLint uniformLocation = glGetUniformLocation(m_shader, "modelmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &ctm[0][0]);


    uniformLocation = glGetUniformLocation(m_shader, "viewmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

    uniformLocation = glGetUniformLocation(m_shader, "projmatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);

    uniformLocation = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(uniformLocation, m_ka);

    uniformLocation = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(uniformLocation, m_kd);

    uniformLocation = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(uniformLocation, m_ks);

    uniformLocation = glGetUniformLocation(m_shader, "cAmbient");
    glUniform4f(uniformLocation, cAmbient[0], cAmbient[1], cAmbient[2], cAmbient[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cDiffuse");
    glUniform4f(uniformLocation, cDiffuse[0], cDiffuse[1], cDiffuse[2], cDiffuse[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cSpecular");
    glUniform4f(uniformLocation, cSpecular[0], cSpecular[1], cSpecular[2], cSpecular[3]);

    uniformLocation = glGetUniformLocation(m_shader, "cameraPos");
    glUniform4f(uniformLocation, cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[3]);

    uniformLocation = glGetUniformLocation(m_shader, "shininess");
    glUniform1f(uniformLocation, shininess);

    uniformLocation = glGetUniformLocation(m_shader, "numLights");
    glUniform1i(uniformLocation, numLights);

    for (int i = 0; i < sceneData.lights.size(); i++) {

        std::string str =  "lightTypes[" + std::to_string(i) + "]";
        uniformLocation = glGetUniformLocation(m_shader, str.c_str());
        glUniform1i(uniformLocation, lightTypes[i]);

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


    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintGL() {
    //  Students: anything requiring OpenGL calls every frame should be done here
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width*  m_devicePixelRatio, m_height * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (sceneLoaded) Realtime::draw(sceneData.shapes[0], true);  // draw the ball
    for (RenderShapeData shape : sceneData.shapes) {
        Realtime::draw(shape, false);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_FireSystem.Emit(m_Fire);
    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter);
}

void Realtime::paintTexture(GLuint texture, bool perPixel, bool kernel){
    glUseProgram(m_texture_shader);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glUseProgram(m_texture_shader);
    GLint uniformLocation = glGetUniformLocation(m_texture_shader, "perPixel");
    glUniform1i(uniformLocation, perPixel);
    uniformLocation = glGetUniformLocation(m_texture_shader, "kernel");
    glUniform1i(uniformLocation, kernel);
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
    extractInfo(settings.sceneFilePath, sceneData);
    SceneCameraData cData = sceneData.cameraData;
    cData.look = ball.getPos() - cData.pos;
    camera = Camera(cData, m_width, m_height);
    drawFire();
}

void Realtime::settingsChanged() {
    if (initialized) {
        Realtime::getVaos();
        update(); // asks for a PaintGL() call to occur
    }
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


void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();
    SceneCameraData cData = camera.getData();
    glm::mat4 ctm = ball.getCTM();

    // Use deltaTime and m_keyMap here to move around
    glm::vec3 look = glm::vec3(glm::normalize(cData.look));
    glm::vec3 up = glm::vec3(glm::normalize(cData.up));
    glm::vec3 left = glm::normalize(glm::cross(up, look));
    float speed;
    speed = m_keyMap[Qt::Key_Shift]? 10.0f : 3.0f;
    float dist = speed * deltaTime;

    // project the moving direction to the plane. This is the simple version, subjecting to change.
    glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 forward = look - glm::dot(look, n) * n;
    left = left - glm::dot(left, n) * n;

    glm::vec3 dir = glm::vec3(0.0);
    glm::vec3 rotAxis = glm::vec3(0.0);

    if (m_keyMap[Qt::Key_W]) {
        dir += forward;
        rotAxis += left;
    }

    if (m_keyMap[Qt::Key_S]) {
        dir -= forward;
        rotAxis -= left;
    }

    if (m_keyMap[Qt::Key_A]) {
        dir += left;
        rotAxis -= forward;
    }

    if (m_keyMap[Qt::Key_D]) {
        dir -= left;
        rotAxis += forward;
    }

    if (m_keyMap[Qt::Key_Control]) {
        if (glm::length(ball.getPos()-cData.pos) >= 0.1) {
            glm::mat4 trans = glm::translate(glm::mat4(1.0), look * dist);
            cData.pos = trans * cData.pos;
        }
    }

    if (m_keyMap[Qt::Key_Space]) {
        glm::mat4 trans = glm::translate(glm::mat4(1.0), -look * dist);
        cData.pos = trans * cData.pos;
    }

    if (dir != glm::vec3(0.0) && rotAxis != glm::vec3(0.0)) {
        dir = glm::normalize(dir);
        rotAxis = glm::normalize(rotAxis);

        glm::mat4 trans = glm::translate(glm::mat4(1.0), dir * dist);
        float theta = glm::length(dir * dist) / ball.getRadius();
        cData.pos = trans * cData.pos;
        ctm = trans * ctm;
        glm::vec3 realPos = glm::vec3(ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.00f));
        glm::mat4 rot = glm::translate(glm::rotate(glm::translate(glm::mat4(1.0), realPos), theta, rotAxis), -realPos);
        ctm = rot * ctm;
    }

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
