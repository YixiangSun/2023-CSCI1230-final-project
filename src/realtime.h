#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "ball/ball.h"
#include "shapes/Water.h"
#include "utils/objparser.h"
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    float m_devicePixelRatio;

    int m_width;
    int m_height;
    GLuint m_shader;
    GLuint m_texture_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_scene_vbo;
    GLuint m_scene_vao;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fbo;

    float m_ka;
    float m_kd;
    float m_ks;
    std::vector<int> lightTypes;
    std::vector<glm::vec4> lightDirs;
    std::vector<glm::vec4> lightPoses;
    std::vector<glm::vec4> lightColors;
    std::vector<glm::vec3> functions;
    std::vector<bool> isFires;
    std::vector<float> angles;
    std::vector<float> penumbras;

    std::vector<std::vector<float>> vertsList;          // Store the vertices data (position + normal) of all the shapes
    bool sceneLoaded = false;                           // check if there's a scene loaded. Used to correctly draw the ball.

    GLint m_defaultFBO = 3; // when no shadow, the defaultFBO is 2

    bool initialized = false;

    void draw(RenderShapeData& shape, bool ifBall, glm::mat4 originalCTM);
    void getVaos();
    void extractInfo(std::string filepath);
    void getFullScreenVao();
    void makeFBO();
    void paintTexture(GLuint texture);

    // scene parsing
    std::vector<float> m_scene_vertices;
    std::vector<std::string> split(std::string& str, char delimiter);
    void makeCorner(std::string corner, std::vector<float> v, std::vector<float> vt,
                    std::vector<float> vn, std::vector<float> vertices);
    std::vector<float> loadMesh(std::string filename);

    // water
    Water water;
    GLuint m_water_vao;
    GLuint m_water_vbo;
    std::vector<float> m_waterData;
    void updateWater(bool initialized);
    glm::vec4 m_topLeft;
    glm::vec4 m_topRight;
    glm::vec4 m_bottomRight;
    glm::vec4 m_bottomLeft;
    float m_rim_width = 1;

    // ball
    SceneMaterial m_ballMaterial;
    glm::vec3 getDir(bool w, bool s, bool a, bool d);
    float m_bound = 75.0;
    bool isInWater();
    glm::vec3 getWaterNormal();
    float time_on_fire = -3.0f;
    bool soaked = false;
    void updateBallAndFireStates(float deltaTime, glm::mat4 &ctm, SceneCameraData &cData);
    bool onRock = false;
    bool onFire = false;

    // fire
    float m_fire_radius = 1.2f;
    glm::vec3 m_fire_pos;
    glm::vec3 m_fire_center;
    bool fireOn = true;

    // texture
    void readTexture();
    QImage m_image;
    QImage m_angry_image;
    GLuint m_kitten_texture;
    GLuint m_angry_kitten_texture;

    // objparser
    OBJparser objparser;
    void paintObject();
    GLuint m_object_shader;
    void createObjVAO();
    void getObjVaos();
    void paintObj();

    // hit objects
    std::vector<RenderShapeData> hitObjs;
    void populateHitObjs();

    // shadow
    GLuint m_shadowMap;
    GLuint m_shadowFBO;
    GLuint m_shadow_shader;
    void makeShadowFBO();
    void ShadowMapPass();

    glm::mat4 LightView;
    glm::mat4 LightProjection;
};

