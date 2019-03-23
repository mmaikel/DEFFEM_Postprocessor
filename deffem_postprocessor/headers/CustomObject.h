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
        CustomObject(std::vector<float> vertices, std::vector<unsigned int> indices,
            std::vector<unsigned int> attributes, const int stride);

        CustomObject(std::vector<float> vertices, std::vector<unsigned int> attributes, int stride);

        void draw(Shader* shader) override;

        void draw() override;

        ~CustomObject();

    private:
        int indicesSize;
        int verticesSize;
    };
}



#endif
