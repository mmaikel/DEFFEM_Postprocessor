#ifndef OBJECT_H
#define OBJECT_H

#include "Shader.h"
#include <GL/gl.h>

namespace deffem
{
    class Object
    {
    public:
        virtual ~Object() = default;

        virtual void draw() = 0;

        virtual void draw(Shader* shader) = 0;

        virtual void destroy() = 0;

    protected:

        GLuint VAO, VBO, EBO = 0;
    };
}
#endif
