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
    class MeshPlane
    {
    public:
        MeshPlane(glm::vec3 center, glm::fvec2 size, int gap, deffem::Color meshColor);

        ~MeshPlane();

        void draw(deffem::Shader* shader);

        void draw();

    private:

        std::list<Line> lines;
    };
}

#endif
