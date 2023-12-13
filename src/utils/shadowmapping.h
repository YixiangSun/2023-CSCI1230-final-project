#ifndef SHADOWMAPPING_H
#define SHADOWMAPPING_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class shadowMapping
{
public:
    shadowMapping();
    ~shadowMapping();

    bool init(unsigned int width, unsigned int height);
    void bindWriteFbo();
    void bindReadFbo();

private:
    unsigned int m_width = 0;
    unsigned int m_height = 0;
    GLuint m_shadowFBO;
    GLuint m_shadowMap;
};

#endif // SHADOWMAPPING_H
