#include <glm/glm/detail/type_vec3.hpp>
#include "../headers/Line.h"

class Axes
{
public:

    Axes()
    {
        deffem::Line axisX(glm::vec3(-0.1f, 0.0f, 0.0f), glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        deffem::Line axisY(glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        deffem::Line axisZ(glm::vec3(0.0f, 0.0f, -0.1f), glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.0f, 0.0f, 1.0f));

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
