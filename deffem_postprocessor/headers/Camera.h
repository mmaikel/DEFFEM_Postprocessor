#pragma once
#ifndef CEMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>


namespace deffem
{
    class Camera
    {
    public:

        GLfloat theta;
        GLfloat phi;
        GLfloat radius;
        GLfloat fov;

        glm::fvec2 offset;
        glm::fvec3 target;

        Camera();
    };
}

#endif
