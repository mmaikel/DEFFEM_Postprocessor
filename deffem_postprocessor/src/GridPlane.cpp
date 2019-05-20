#include <list>
#include "../headers/GridPlane.h"
#include <functional>

using namespace deffem;


GridPlane::GridPlane()
{
    showGrid = false;

    axisX = new Line(glm::vec3(-0.1f, 0.0001f, 0.0f), glm::vec3(0.1f, 0.0001f, 0.0f), Colors::red);
    axisY = new Line(glm::vec3(-0.0001f, -0.1f, 0.0f), glm::vec3(-0.0001f, 0.1f, 0.0f), Colors::green);
    axisZ = new Line(glm::vec3(0.0f, 0.0001f, -0.1f), glm::vec3(0.0f, 0.0001f, 0.1f), Colors::blue);
}


GridPlane::GridPlane(const glm::fvec2 size, const int grids, const Color& meshColor)
{
    showGrid = true;
    const auto dx = size.x / grids;

    for (auto i = 0; i <= grids; i++)
    {
        const auto pos = i * dx - size.x / 2;

        if (pos == 0.0f)
        {
            Line line1(glm::vec3(pos, 0.0f, 0.0f - (size.y / 2)), glm::vec3(pos, 0.0f, -0.1f),
                       meshColor);
            Line line2(glm::vec3(0.0f - (size.y / 2), 0.0f, pos), glm::vec3(-0.1f, 0.0f, pos),
                       meshColor);

            Line line3(glm::vec3(pos, 0.0f, 0.1f), glm::vec3(pos, 0.0f, size.y / 2),
                       meshColor);
            Line line4(glm::vec3(0.1f, 0.0f, pos), glm::vec3(size.y / 2, 0.0f, pos),
                       meshColor);

            gridLines.push_back(line1);
            gridLines.push_back(line2);
            gridLines.push_back(line3);
            gridLines.push_back(line4);
        }
        else
        {
            Line line1(glm::vec3(pos, 0.0f, 0.0f - (size.y / 2)), glm::vec3(pos, 0.0f, size.y / 2),
                       meshColor);
            Line line2(glm::vec3(0.0f - (size.y / 2), 0.0f, pos), glm::vec3(size.y / 2, 0.0f, pos),
                       meshColor);

            gridLines.push_back(line1);
            gridLines.push_back(line2);
        }
    }

    axisX = new Line(glm::vec3(-0.1f, 0.0001f, 0.0f), glm::vec3(0.1f, 0.0001f, 0.0f), Colors::red);
    axisY = new Line(glm::vec3(-0.0001f, -0.1f, 0.0f), glm::vec3(-0.0001f, 0.1f, 0.0f), Colors::green);
    axisZ = new Line(glm::vec3(0.0f, 0.0001f, -0.1f), glm::vec3(0.0f, 0.0001f, 0.1f), Colors::blue);
}

GridPlane::~GridPlane()
{
    for (auto& line : gridLines)
    {
        line.destroy();
    }

    axisX->destroy();
    axisY->destroy();
    axisZ->destroy();

    delete axisX;
    delete axisY;
    delete axisZ;
}


void GridPlane::draw(Shader* shader, const GLfloat lineWidth)
{
    if (showGrid)
    {
        glLineWidth(0.5);
        for (auto& line : gridLines)
        {
            line.draw(shader);
        }
    }

    glLineWidth(lineWidth);

    axisX->draw(shader);
    axisY->draw(shader);
    axisZ->draw(shader);
}

void GridPlane::draw()
{
    if (showGrid)
    {
        for (auto& line : gridLines)
        {
            line.draw();
        }
    }

    axisX->draw();
    axisY->draw();
    axisZ->draw();
}
