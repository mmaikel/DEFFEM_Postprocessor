#pragma once
#ifndef CUSTOM_OBJECT_H 
#define CUSTOM_OBJECT_H
#include "Object.h"
#include <vector>


namespace deffem
{
    class CustomObject final : protected Object
    {
    public:
        CustomObject(std::vector<GLfloat> vertices, std::vector<GLuint> indices,
            std::vector<GLuint> attributes, int stride);

        CustomObject(std::vector<GLfloat> vertices, std::vector<GLuint> attributes, int stride);

        void draw(Shader* shader) override;

        void draw() override;

        void destroy() override;

        ~CustomObject();

    private:
        int indicesSize_;
        int verticesSize_;
    };
}



#endif
