#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <glad/glad.h>
#include <string>
#include <map>

namespace deffem
{
    class Color
    {
    public:

        Color();

        Color(GLfloat red, GLfloat green, GLfloat blue);

        Color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

        Color(GLfloat red, GLfloat green, GLfloat blue, std::string name);

        Color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, std::string name);


        static Color fromString(std::string colorName);

        std::string colorName;

        GLfloat red, green, blue, alpha;
    };

    class Colors
    {
    public:

        static std::map<std::string, Color> colors;

        static Color red;

        static Color green;

        static Color blue;

        static Color lightBlue;

        static Color yellow;

        static Color magenta;

        static Color purple;

        static Color black;

        static Color gray;

        static Color lightGray;

        static Color darkGray;

        static Color white;

        static Color orange;
    };
}

#endif
