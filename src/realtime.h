#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "ball/ball.h"
#include "shapes/Water.h"
#include "utils/rgba.h"
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
    std::vector<float> angles;
    std::vector<float> penumbras;

    std::vector<std::vector<float>> vertsList;          // Store the vertices data (position + normal) of all the shapes
    bool sceneLoaded = false;                           // check if there's a scene loaded. Used to correctly draw the ball.

    GLint m_defaultFBO = 2;

    bool initialized = false;

    void draw(RenderShapeData& shape, bool ifBall);
    void getVaos();
    void extractInfo(std::string filepath);
    void getFullScreenVao();
    void makeFBO();
    void paintTexture(GLuint texture, bool perPixel, bool kernel);

    // water
    Water water;
    GLuint m_water_vao;
    GLuint m_water_vbo;
    std::vector<float> m_waterData;
    void updateWater(bool initialized);

    // ball
    SceneMaterial m_ballMaterial;
    Texture m_ballTexture;
};

