#pragma once
#ifndef HEATMAP_H
#define HEATMAP_H
#include <glm/glm/detail/type_vec3.hpp>
#include "Typer.h"
#include "ModelInfo.h"
#include <list>
#include "CustomObject.h"
#include "Rectangle.h"

namespace deffem
{
    class Heatmap
    {
    public:

        Heatmap(glm::fvec2 pos, glm::fvec2 size, ModelInfo modelInfo, const Color& textColor);

        ~Heatmap();

        void draw(Shader* shader, Shader* textShader, Typer* typer);

        void getHeatMapColor(float value, float* red, float* green, float* blue) const;

        void setProjection(glm::fmat4* projection);

        void setPosition(glm::fvec2 pos);


    private:

        std::vector<float> valPoints;

        glm::fmat4* projection;
        glm::vec2 position;
        CustomObject* heatmap;
        std::list<std::pair<glm::fvec4, Rectangle>> heatMapRows;
        ModelInfo modelInfo;
        Color textColor;
    };
}

#endif
