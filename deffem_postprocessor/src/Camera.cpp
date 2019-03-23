#include "../headers/Camera.h"

using namespace deffem;

Camera::Camera()
{
    this->phi = 1.57;
    this->theta = 0;
    this->fov = 25.0;
    this->offset = glm::fvec2(0.0);
    this->radius = 0.25;
    this->target = glm::fvec3(0.0, 0.033, 0.0);
}
