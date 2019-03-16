#include <list>
#include "../headers/Line.h"

class MeshPlane
{
public:

    MeshPlane(glm::vec3 center, float width, float height, int gap)
    {
        auto dx = width / gap;


        for (auto i = 0; i <= gap; i++)
        {
            auto pos = i * dx - width / 2;


            deffem::Line line1(glm::vec3(pos, 0.0f, 0.0f - (height / 2)), glm::vec3(pos, 0.0f, height / 2),
                               glm::vec3(0.1f, 0.1f, 0.1f));
            deffem::Line line2(glm::vec3(0.0f - (height / 2), 0.0f, pos), glm::vec3(height / 2, 0.0f, pos),
                               glm::vec3(0.1f, 0.1f, 0.1f));

            lines.push_front(line1);
            lines.push_front(line2);
        }

        deffem::Line axisX(glm::vec3(-0.1f, 0.0001f, 0.0f), glm::vec3(0.1f, 0.0001f, 0.0f), glm::vec3(1.0f, 0.0001f, 0.0f));
        deffem::Line axisY(glm::vec3(-0.0001f, -0.1f, 0.0f), glm::vec3(-0.0001f, 0.1f, 0.0f), glm::vec3(-0.0001f, 1.0f, 0.0f));
        deffem::Line axisZ(glm::vec3(0.0f, 0.0001f, -0.1f), glm::vec3(0.0f, 0.0001f, 0.1f), glm::vec3(0.0f, 0.0001f, 1.0f));

        lines.push_back(axisX);
        lines.push_back(axisY);
        lines.push_back(axisZ);
    }


    void draw(Shader* shader)
    {
        for (auto line : lines)
        {
            line.draw(shader);
        }
    }

    void draw()
    {
        for (auto line : lines)
        {
            line.draw();
        }
    }

protected:

    std::list<deffem::Line> lines;
};
