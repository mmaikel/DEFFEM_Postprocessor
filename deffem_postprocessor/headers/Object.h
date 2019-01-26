#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include "Shader.h"


class Object
{
public:

    ~Object()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    virtual void draw() = 0;

    virtual void draw(Shader shader) = 0;

protected:

    GLuint VAO, VBO, EBO = 0;
    GLfloat* vertices;
    GLint* indices;
};

#endif
