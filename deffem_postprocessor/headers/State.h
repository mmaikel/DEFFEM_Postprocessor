#pragma once


#ifndef STATE_H
#define STATE_H
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include "Camera.h"
#include "Typer.h"
#include "Heatmap.h"
#include "ModelContext.h"
#include "ApplicationSettings.h"
#include "AnimationState.h"
#include "MouseState.h"

namespace deffem {

    class State
    {
    public:

        glm::fvec2 screenSize;

        glm::fmat4 HUDprojection;

        MouseState mouse;

        Camera camera;

        Heatmap* heatmap;

        std::vector<ModelContext*> deffemModelContexts;

        size_t currentModelIndex;

        AnimationState animation;

        GLfloat modelScale;

        ApplicationSettings* settings;

        State(ApplicationSettings* settings);

        ~State();

        void clearModels();
        void refresh();

        bool animationTick();
        void restartAnimation();
        bool nextModel();
        bool previousModel();
        bool changeModel(size_t index);
        ModelContext* currentModelContext();

        void displayStateInfoText(Shader* textShader, Typer* typer);
    };
}

#endif
