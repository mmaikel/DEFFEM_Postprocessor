#pragma once
#ifndef MESH_PLANE_H
#define MESH_PLANE_H

#include "Color.h"
#include "Shader.h"
#include <glm/glm/glm.hpp>
#include "Line.h"
#include <list>

namespace deffem
{
    class GridPlane
    {
    public:

        bool showGrid;

        GridPlane();

        GridPlane(glm::fvec2 size, int grids, const Color& meshColor);

        ~GridPlane();

        void draw(Shader* shader, GLfloat gridLineWidth);

        void draw();

    private:

        std::list<Line> gridLines;
        Line* axisX;
        Line* axisY;
        Line* axisZ;
    };
}

#endif
