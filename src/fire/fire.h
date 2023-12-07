#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <camera/camera.h>
struct FireProps
{
    glm::vec3 Position;
    glm::vec3 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
};

class Fire
{
public:
    Fire();

    void OnUpdate(float timestep);
    void OnRender(Camera camera);

    void Emit(const FireProps& fireProps);
private:
    struct FireParticle
    {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;

        bool Active = false;
    };
    std::vector<FireParticle> m_FirePool;
    uint32_t m_PoolIndex = 999;

    GLuint m_FireVA = 0;
    GLuint fireVB, fireIB;
    GLuint m_FireShader;
    GLuint m_FireShaderViewProj, m_FireShaderTransform, m_FireShaderColor;

};


