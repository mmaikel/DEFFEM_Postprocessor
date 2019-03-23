#pragma once
#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "Object.h"
#include "Color.h"


namespace deffem
{
    class Rectangle : protected Object
    {
    public:

        Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);

        Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, Color color);

        Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, Color color[4]);

        ~Rectangle();

        void draw(Shader* shader) override;

        void draw() override;
    };
}

#endif
