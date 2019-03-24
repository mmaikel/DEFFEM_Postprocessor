#include <vector>
#include <map>
#include <list>
#include "../headers/Heatmap.h"
#include "../headers/Rectangle.h"
#include "enumerate.cpp"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

using namespace deffem;
using namespace std;
using namespace glm;


Heatmap::Heatmap(const fvec2 pos, const fvec2 size, const ModelInfo modelInfo, const Color& textColor)
{
    this->textColor = textColor;
    position = pos;

    const auto localPosition = fvec2(0.0f);

    this->modelInfo = modelInfo;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const auto min = modelInfo.minMaxValue.min;
    const auto max = modelInfo.minMaxValue.max;

    const auto precision = 10;
    const auto heightGap = size.y / precision;
    const auto valueGap = (max - min) / precision;
    float red, green, blue;

    for (auto i = 0; i < precision * 2; i++)
    {
        const auto val = (i * valueGap) + min;
        const auto normalizedVal = (val - min) / (max - min);
        getHeatMapColor(normalizedVal, &red, &green, &blue);

        vertices.push_back(localPosition.x);
        vertices.push_back(localPosition.y + (heightGap * i));
        vertices.push_back(0.0f);
        vertices.push_back(red);
        vertices.push_back(green);
        vertices.push_back(blue);

        vertices.push_back(localPosition.x + size.x);
        vertices.push_back(localPosition.y + (heightGap * i));
        vertices.push_back(0.0f);
        vertices.push_back(red);
        vertices.push_back(green);
        vertices.push_back(blue);

        if (i % 2 == 0)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + 3);
        }
        else
        {
            indices.push_back(i + 1);
            indices.push_back(i + 2);
            indices.push_back(i - 1);
        }

        valPoints.push_back(localPosition.x + size.x);
        valPoints.push_back(localPosition.y + (heightGap * i));
        valPoints.push_back(0.0f);

        if (i <= 10)
        {
            const auto rowPosition = fvec2(localPosition.x + size.x + 5.0f, localPosition.y + (heightGap * i));
            auto rect = Rectangle(rowPosition.x, rowPosition.y, 0.0, 15.0f, 1.0f, textColor);
            auto p = fvec4(rowPosition.x, rowPosition.y, 0.0f, val);
            heatMapRows.push_front(std::pair<fvec4, Rectangle>(p, rect));
        }
    }

    std::vector<unsigned int> attribsSizes;
    attribsSizes.push_back(3);
    attribsSizes.push_back(3);

    heatmap = new deffem::CustomObject(vertices, indices, attribsSizes, 6);
}

void Heatmap::getHeatMapColor(float value, float* red, float* green, float* blue) const
{
    const auto numColors = 4;
    static float color[numColors][3] = {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1; // |-- Our desired color will be between these two indexes in "color".
    int idx2; // |
    float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

    if (value <= 0) { idx1 = idx2 = 0; } // accounts for an input <=0
    else if (value >= 1) { idx1 = idx2 = numColors - 1; } // accounts for an input >=0
    else
    {
        value = value * (numColors - 1); // Will multiply value by 3.
        idx1 = floor(value); // Our desired color will be after this index.
        idx2 = idx1 + 1; // ... and before this index (inclusive).
        fractBetween = value - float(idx1); // Distance between the two indexes (0-1).
    }

    *red = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
    *green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
    *blue = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
}

Heatmap::~Heatmap()
{
    delete heatmap;
    for (auto& row : heatMapRows)
    {
        row.second.destroy();
    }
}


void Heatmap::draw(Shader* shader, Shader* tShader, Typer* typer)
{
    const auto translate = glm::translate(*projection, glm::fvec3(position.x, position.y, 0.0));

    shader->use();
    shader->setMat4("projection", translate);

   

    tShader->use();
    tShader->setMat4("projection", translate);

    heatmap->draw(shader);

    for (const auto& row : heatMapRows)
    {
        const auto posVal = row.first;
        auto rect = row.second;
        rect.draw(shader);
        typer->renderText(tShader, to_string(posVal.w), fvec2(posVal.x + 25.0f, posVal.y), 0.3f, textColor);
    }

    typer->renderText(tShader, "Node count:", fvec2(0, 0 - 25.0f), 0.3f, textColor);
    typer->renderText(tShader, "Element count:", fvec2(0, 0 - 50.0f), 0.3f, textColor);

    typer->renderText(tShader, to_string(modelInfo.elementCount), fvec2(0 + 105.0, 0 - 50.0f), 0.3f,
        textColor);
    typer->renderText(tShader, to_string(modelInfo.nodeCount), fvec2(0 + 105.0, 0 - 25.0f), 0.3f,
        textColor);
}

// Sets new window size
void Heatmap::setProjection(glm::fmat4* projection)
{
    this->projection = projection;
}


// Sets new position
void Heatmap::setPosition(glm::fvec2 pos)
{
    position = pos;
}
