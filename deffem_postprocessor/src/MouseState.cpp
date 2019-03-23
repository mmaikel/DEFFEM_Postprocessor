#include "../headers/MouseState.h"
#include <GLFW/glfw3.h>

using namespace deffem;

MouseState::MouseState()
{
    this->isPressed = false;
    this->isPressedWithControl = false;
    this->isPressedWithShift = false;
    this->position = glm::fvec2(0.0);
}

void MouseState::pressed()
{
    this->isPressed = true;
    this->isPressedWithControl = false;
    this->isPressedWithShift = false;
}

void MouseState::pressedWith(const int mod)
{
    switch (mod)
    {
    case GLFW_MOD_CONTROL:
        this->isPressed = false;
        this->isPressedWithControl = true;
        this->isPressedWithShift = false;
        break;

    case GLFW_MOD_SHIFT:
        this->isPressed = false;
        this->isPressedWithControl = false;
        this->isPressedWithShift = true;
        break;

    default:
        this->isPressed = false;
        this->isPressedWithControl = false;
        this->isPressedWithShift = false;
    }
}

void MouseState::released()
{
    this->isPressed = false;
    this->isPressedWithControl = false;
    this->isPressedWithShift = false;
}
