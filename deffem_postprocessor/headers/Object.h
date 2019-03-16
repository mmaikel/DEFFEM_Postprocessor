#ifndef OBJECT_H
#define OBJECT_H

#include "Shader.h"
#include <list>
#include <GL/gl.h>


class Object
{
public:

  

    virtual void draw() = 0;

    virtual void draw(Shader *shader) = 0;

protected:

    GLuint VAO, VBO, EBO = 0;
};

#endif
