#pragma once
#ifndef HEATMAP_H
#define HEATMAP_H
#include <glm/glm/detail/type_vec3.hpp>
#include "Typer.h"
#include "ModelInfo.h"
#include <list>
#include "CustomObject.h"

namespace deffem
{
    class Heatmap
    {
    public:

        Heatmap(glm::fvec2 pos, glm::fvec2 size, ModelInfo modelInfo, Color textColor);

        ~Heatmap();

        void draw(Shader* shader, Shader* textShader, Typer* typer);

        void getHeatMapColor(float value, float* red, float* green, float* blue);

    private:

        std::vector<float> valPoints;
        glm::vec3 position;
        CustomObject* heatmap;
        std::list<glm::vec4> valuesPositions;
        ModelInfo modelInfo;
        Color textColor;
    };
}

#endif
