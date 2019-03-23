#include "../headers/Color.h"

using namespace deffem;

Color::Color()
{
    this->red = 0.0;
    this->green = 0.0;
    this->blue = 0.0;
    this->alpha = 1.0;
}


Color::Color(GLfloat red, GLfloat green, GLfloat blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = 1.0;
}

Color::Color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

Color::Color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, std::string name)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = 1.0;
    this->colorName = name;
}

Color::Color(GLfloat red, GLfloat green, GLfloat blue, std::string name)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
    this->colorName = name;
}


Color Color::fromString(std::string colorName)
{
    return Colors::colors[colorName];
}

Color Colors::red(1.0, 0.0, 0.0, "red");

Color Colors::green(0.0, 1.0, 0.0, "green");

Color Colors::blue(0.0, 0.0, 1.0, "blue");

Color Colors::black(0.0, 0.0, 0.0, "black");

Color Colors::darkGray(0.15f, 0.15f, 0.17f, "dark-gray");

Color Colors::gray(0.3, 0.3, 0.32, "gray");

Color Colors::lightGray(0.55f, 0.55f, 0.57f, "light-gray");

Color Colors::lightBlue(0.53, 0.8, 0.98, "light-blue");

Color Colors::white(1.0, 1.0, 1.0, "white");

Color Colors::yellow(1.0, 1.0, 0.4, "yellow");

Color Colors::orange(1.0, 0.65, 0.0, "orange");

Color Colors::magenta(1.0, 0.0, 0.564, "magenta");

Color Colors::purple(0.5, 0.0, 0.5, "purple");

std::map<std::string, Color> Colors::colors = std::map<std::string, Color>{
    std::pair<std::string, Color>(red.colorName, red),
    std::pair<std::string, Color>(green.colorName, green),
    std::pair<std::string, Color>(blue.colorName, blue),
    std::pair<std::string, Color>(black.colorName, black),
    std::pair<std::string, Color>(darkGray.colorName, darkGray),
    std::pair<std::string, Color>(gray.colorName, gray),
    std::pair<std::string, Color>(lightGray.colorName, lightGray),
    std::pair<std::string, Color>(lightBlue.colorName, lightBlue),
    std::pair<std::string, Color>(white.colorName, white),
    std::pair<std::string, Color>(yellow.colorName, yellow),
    std::pair<std::string, Color>(orange.colorName, orange),
    std::pair<std::string, Color>(magenta.colorName, magenta),
    std::pair<std::string, Color>(purple.colorName, purple),
};
