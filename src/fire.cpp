#include "fire.h"
//#include "random.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/compatibility.hpp>

#include <QCoreApplication>
#include <iostream>
#include "ball.h"
#include "camera.h"
#include "shaderloader.h"

Fire::Fire()
{
    m_FirePool.resize(1000);
}

void Fire::OnUpdate(float timestep){
    for (auto&particle: m_FirePool){
        if (!particle.Active){
            continue;
        }
        if (particle.LifeRemaining <= 0.0f){
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= timestep;
        particle.Position += particle.Velocity * (float) timestep;
        particle.Rotation += 0.01f * timestep;
    }
}

void Fire::OnRender(Camera camera){
    m_FireShader = ShaderLoader::createShaderProgram(":/resources/shaders/fire.vert", ":/resources/shaders/fire.frag");
    if (!m_FireVA){
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
        };


        glCreateBuffers(1, &fireVB);
        glBindBuffer(GL_ARRAY_BUFFER, fireVB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glCreateVertexArrays(1, &m_FireVA);
        glBindVertexArray(m_FireVA);

        glEnableVertexArrayAttrib(fireVB, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        uint32_t indices[] = {0, 1, 2, 2, 3, 0};

        glCreateBuffers(1, &fireIB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fireIB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        m_FireShaderViewProj = glGetUniformLocation(m_FireShader, "u_ViewProj");
        m_FireShaderTransform = glGetUniformLocation(m_FireShader, "u_Transform");
        m_FireShaderColor = glGetUniformLocation(m_FireShader, "u_Color");
    }
    glUseProgram(m_FireShader);
    glm::mat4 viewProj = camera.getProjMatrix() * camera.getViewMatrix();
    glUniformMatrix4fv(m_FireShaderViewProj, 1, GL_FALSE, &viewProj[0][0]);
    for  (auto& particle: m_FirePool){
        if (!particle.Active){
            continue;
        }

        // Fade away particles
        float life = particle.LifeRemaining / particle.LifeTime;
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        color.a = color.a * life;

        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

        // Render
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), {particle.Position.x, particle.Position.y, 0.0f})
                              * glm::rotate(glm::mat4(1.0f), particle.Rotation, {0.0f, 0.0f, 1.0f})
                              * glm::scale(glm::mat4(1.0f), {size, size, 1.0f});
        glUniformMatrix4fv(m_FireShaderTransform, 1, GL_FALSE, &transform[0][0]);
        glUniform4fv(m_FireShaderColor, 1, &color[0]);
        glBindVertexArray(m_FireVA);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}

void Fire::Emit(const FireProps& fireProps){
    FireParticle& particle = m_FirePool[m_PoolIndex];
    particle.Active = true;
    particle.Position = fireProps.Position;
//    particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();
    particle.Rotation = 0.5f * 2.0f * glm::pi<float>();

    // Velocity
    particle.Velocity = fireProps.Velocity;
//    particle.Velocity.x = fireProps.VelocityVariation.x * (Random::Float() - 0.5f);
//    particle.Velocity.y = fireProps.VelocityVariation.y * (Random::Float() - 0.5f);
    particle.Velocity.x = fireProps.VelocityVariation.x * 0.5f;
    particle.Velocity.y = fireProps.VelocityVariation.y * 0.5f;

    // Color
    particle.ColorBegin = fireProps.ColorBegin;
    particle.ColorEnd = fireProps.ColorEnd;

    particle.LifeTime = fireProps.LifeTime;
    particle.LifeRemaining = fireProps.LifeTime;
//    particle.SizeBegin = fireProps.SizeBegin * fireProps.SizeVariation * (Random::Float() - 0.5f);
     particle.SizeBegin = fireProps.SizeBegin * fireProps.SizeVariation * 0.5f;
    particle.SizeEnd = fireProps.SizeEnd;

    m_PoolIndex = -m_PoolIndex % m_FirePool.size();
}
