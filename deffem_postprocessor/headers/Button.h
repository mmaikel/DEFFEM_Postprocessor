#pragma once
#ifndef BUTTON_H
#define BUTTON_H
#include "Rectangle.h"
#include "Shader.h"

namespace deffem {

    class Button : protected Rectangle
    {
    public:

        Button(glm::fvec2 pos, glm::fvec2 size, std::string texturePath);

        void draw(Shader *shader) override;

        void draw() override;

        void setProjection(glm::fmat4* projection);

        void setPosition(glm::fvec2 pos);

        bool isClicked(glm::fvec2 pos);

        void changeTexture(const std::string& texturePath);

        unsigned int texture;

        ~Button();

        glm::fvec2 position;

        glm::fvec2 size;

        glm::fmat4* projection;

    };

}


#endif
