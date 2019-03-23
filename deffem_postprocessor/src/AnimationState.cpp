#include "../headers/AnimationState.h"

using namespace deffem;

void AnimationState::stop()
{
    play = false;
}

void AnimationState::start()
{
    play = true;
    isFinished = false;
}

