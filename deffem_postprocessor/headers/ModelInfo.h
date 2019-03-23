#pragma once
#ifndef MODELINFO_H
#define MODELINFO_H
#include "MinMax.h"

namespace deffem
{
    class ModelInfo
    {
    public:
        unsigned long nodeCount;
        unsigned long elementCount;
        MinMax minMaxValue;
    };
}

#endif
