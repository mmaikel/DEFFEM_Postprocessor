#include <list>
#include "../headers/MeshPlane.h"
#include <functional>

using namespace deffem;


MeshPlane::MeshPlane(glm::vec3 center, const glm::fvec2 size, const int gap, const Color meshColor)
{
    const auto dx = size.x / gap;

    for (auto i = 0; i <= gap; i++)
    {
        const auto pos = i * dx - size.x / 2;

        Line line1(glm::vec3(pos, 0.0f, 0.0f - (size.y / 2)), glm::vec3(pos, 0.0f, size.y / 2),
                   meshColor);
        Line line2(glm::vec3(0.0f - (size.y / 2), 0.0f, pos), glm::vec3(size.y / 2, 0.0f, pos),
                   meshColor);

        lines.push_back(line2);
        lines.push_back(line1);
    }

    const Line axisX(glm::vec3(-0.1f, 0.0001f, 0.0f), glm::vec3(0.1f, 0.0001f, 0.0f), Colors::red);
    const Line axisY(glm::vec3(-0.0001f, -0.1f, 0.0f), glm::vec3(-0.0001f, 0.1f, 0.0f), Colors::green);
    const Line axisZ(glm::vec3(0.0f, 0.0001f, -0.1f), glm::vec3(0.0f, 0.0001f, 0.1f), Colors::blue);

    lines.push_back(axisX);
    lines.push_back(axisY);
    lines.push_back(axisZ);
}

MeshPlane::~MeshPlane()
{
    for (auto& line : lines)
    {
        line.destroy();
    }
}


void MeshPlane::draw(Shader* shader)
{

    for(auto& line : lines)
    {
        line.draw(shader);
    }  
}

void MeshPlane::draw()
{
    for (auto& line : lines)
    {
        line.draw();
    }
}
