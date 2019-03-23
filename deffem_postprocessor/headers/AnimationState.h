#pragma once
#ifndef ANIMATION_STATE_H
#define ANIMATION_STATE_H
#include <chrono>


namespace deffem
{
    class AnimationState
    {
    public:
        bool play;
        bool isFinished;
        std::chrono::milliseconds nextDeadline;
        std::chrono::milliseconds tickMillis;

        void start();
        void stop();
    };
}

#endif
