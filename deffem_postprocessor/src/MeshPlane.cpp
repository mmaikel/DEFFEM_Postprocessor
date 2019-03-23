#include <list>
#include "../headers/MeshPlane.h"

using namespace deffem;


MeshPlane::MeshPlane(glm::vec3 center, glm::fvec2 size, int gap, Color meshColor)
    {
        auto dx = size.x / gap;


        for (auto i = 0; i <= gap; i++)
        {
            auto pos = i * dx - size.x / 2;


            deffem::Line line1(glm::vec3(pos, 0.0f, 0.0f - (size.y / 2)), glm::vec3(pos, 0.0f, size.y / 2),
                meshColor);
            deffem::Line line2(glm::vec3(0.0f - (size.y / 2), 0.0f, pos), glm::vec3(size.y / 2, 0.0f, pos),
                meshColor);

            lines.push_front(line1);
            lines.push_front(line2);
        }

        deffem::Line axisX(glm::vec3(-0.1f, 0.0001f, 0.0f), glm::vec3(0.1f, 0.0001f, 0.0f), Colors::red);
        deffem::Line axisY(glm::vec3(-0.0001f, -0.1f, 0.0f), glm::vec3(-0.0001f, 0.1f, 0.0f), Colors::green);
        deffem::Line axisZ(glm::vec3(0.0f, 0.0001f, -0.1f), glm::vec3(0.0f, 0.0001f, 0.1f), Colors::blue);

        lines.push_back(axisX);
        lines.push_back(axisY);
        lines.push_back(axisZ);
    }


    void MeshPlane::draw(Shader* shader)
    {
        for (auto line : lines)
        {
            line.draw(shader);
        }
    }

    void MeshPlane::draw()
    {
        for (auto line : lines)
        {
            line.draw();
        }
    }


