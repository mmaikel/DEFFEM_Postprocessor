#pragma once

#ifndef TYPER_H
#define TYPER_H

#include <glad/glad.h>
#include "Shader.h"
#include <string>
#include "Color.h"
#include <map>
#include "Letter.h"

namespace deffem
{
    class Typer
    {
    public:

        Typer();

        ~Typer();

        void renderText(Shader* s, std::string text, glm::fvec2 pos, GLfloat size, Color color);

        void bindTexture(char text);

    private:

        std::map<char, Letter> letters;
        GLuint VAO{}, VBO{}, EBO = 0;
    };
}

#endif
