#include <vector>
#include <map>
#include <list>
#include "../headers/Heatmap.h"
#include "../headers/Rectangle.h"

using namespace deffem;


Heatmap::Heatmap(glm::fvec2 pos, glm::fvec2 size, ModelInfo modelInfo, Color textColor)
{
    this->textColor = textColor;
    position = glm::vec3(pos.x, pos.y, 0.0f);
  
    this->modelInfo = modelInfo;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    auto min = modelInfo.minMaxValue.min;
    auto max = modelInfo.minMaxValue.max;

    int precision = 10;
    float heightGap = size.y / precision;
    float valueGap = (max - min) / precision;
    float red, green, blue;

    for (auto i = 0; i < precision * 2; i++)
    {
        auto val = (i * valueGap) + min;
        const float normalizedVal = (val - min) / (max - min);
        getHeatMapColor(normalizedVal, &red, &green, &blue);

        vertices.push_back(pos.x);
        vertices.push_back(pos.y + (heightGap * i));
        vertices.push_back(0.0f);
        vertices.push_back(red);
        vertices.push_back(green);
        vertices.push_back(blue);

        vertices.push_back(pos.x + size.x);
        vertices.push_back(pos.y + (heightGap * i));
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

        valPoints.push_back(pos.x + size.x);
        valPoints.push_back(pos.y + (heightGap * i));
        valPoints.push_back(0.0f);

        if (i <= 10)
        {
            valuesPositions.push_front(glm::vec4(pos.x + size.x + 5.0f, pos.y + (heightGap * i), 0.0f, val));
        }
    }

    std::vector<unsigned int> attribsSizes;
    attribsSizes.push_back(3);
    attribsSizes.push_back(3);

    heatmap = new deffem::CustomObject(vertices, indices, attribsSizes, 6);
}

void Heatmap::getHeatMapColor(float value, float* red, float* green, float* blue)
{
    const int NUM_COLORS = 4;
    static float color[NUM_COLORS][3] = {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1; // |-- Our desired color will be between these two indexes in "color".
    int idx2; // |
    float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

    if (value <= 0) { idx1 = idx2 = 0; } // accounts for an input <=0
    else if (value >= 1) { idx1 = idx2 = NUM_COLORS - 1; } // accounts for an input >=0
    else
    {
        value = value * (NUM_COLORS - 1); // Will multiply value by 3.
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
}


void Heatmap::draw(Shader* shader, Shader* tShader, Typer* typer)
{
    typer->renderText(tShader, "Node count:", glm::fvec2(position.x, position.y - 25.0f), 0.3f,
                      textColor);
    typer->renderText(tShader, std::to_string(modelInfo.nodeCount), glm::fvec2(position.x + 105.0, position.y - 25.0f), 0.3f,
                      textColor);
    typer->renderText(tShader, "Element count:", glm::fvec2(position.x, position.y - 50.0f), 0.3f,
                      textColor);
    typer->renderText(tShader, std::to_string(modelInfo.elementCount), glm::fvec2(position.x + 105.0, position.y - 50.0f),
                      0.3f,
                      textColor);


    heatmap->draw(shader);
    for (auto pos : valuesPositions)
    {
        deffem::Rectangle rec(pos.x, pos.y, pos.z, 15.0f, 1.0f, textColor);
        rec.draw(shader);
        typer->renderText(tShader, std::to_string(pos.w), glm::fvec2(pos.x + 25.0f, pos.y), 0.3f,
                          textColor);
    }
}


