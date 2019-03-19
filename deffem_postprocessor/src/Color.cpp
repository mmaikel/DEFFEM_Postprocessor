#include <glad/glad.h>


class Color
{
public:
    Color(GLfloat red, GLfloat green, GLfloat blue)
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    GLfloat red, green, blue;
};
