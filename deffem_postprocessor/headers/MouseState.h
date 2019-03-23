#pragma once
#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H
#include <glm/glm/glm.hpp>

namespace deffem
{
    class MouseState
    {
    public:
        glm::fvec2 position;

        bool isPressed;
        bool isPressedWithShift;
        bool isPressedWithControl;

        MouseState();

        void released();
        void pressed();
        void pressedWith(int mod);
    };
}


#endif
