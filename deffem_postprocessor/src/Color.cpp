#include "../headers/Color.h"


Color::Color(GLfloat red, GLfloat green, GLfloat blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

Color Color::fromString(std::string colorName)
{
    if (colorName == "white")
    {
        return Color(1.0, 1.0, 1.0);
    }
    else if (colorName == "black")
    {
        return Color(0.0, 0.0, 0.0);
    }
    else if (colorName == "dark_gray")
    {
        return Color(0.15f, 0.15f, 0.17f);
    }
    else if (colorName == "light_gray")
    {
        return Color(0.55f, 0.55f, 0.57f);
    }
}
