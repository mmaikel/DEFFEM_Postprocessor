#include "../headers/Camera.h"

using namespace deffem;

Camera::Camera()
{
    this->phi = 1.57f;
    this->theta = 0;
    this->fov = 25.0f;
    this->offset = glm::fvec2(0.0f);
    this->radius = 0.25f;
    this->target = glm::fvec3(0.0f, 0.033f, 0.0f);
}
