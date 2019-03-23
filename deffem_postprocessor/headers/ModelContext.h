#pragma once
#ifndef MODEL_CONTEXT_H
#define MODEL_CONTEXT_H

#include <string>
#include "ModelInfo.h"
#include "CustomObject.h"

namespace deffem
{
    class ModelContext
    {
    public:
        CustomObject* model;
        ModelInfo info;
        std::string filename;

        ModelContext(CustomObject* modelPtr, ModelInfo info, std::string filename)
        {
            this->model = modelPtr;
            this->info = info;
            this->filename = filename;
        }

        ~ModelContext()
        {
            delete model;
        }
    };
}

#endif
