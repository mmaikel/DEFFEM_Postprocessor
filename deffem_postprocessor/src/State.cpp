#include "../headers/State.h"
#include <chrono>
#include <glm/glm/gtc/matrix_transform.hpp>


using namespace std::chrono;
using namespace std;
using namespace deffem;
using namespace glm;


State::State(ApplicationSettings* settings)
{
    this->settings = settings;
    this->screenSize = settings->screenResolution;
    this->mouse = MouseState();
    this->heatmap = nullptr;
    this->camera = Camera();
    this->camera.radius = settings->cameraRadius;
    this->currentModelIndex = 0;
    this->animation = AnimationState();
    this->animation.tickMillis = milliseconds(settings->animationTickMillis);
    this->animation.repeat = false;
    this->modelScale = settings->modelScale;
    this->HUDprojection = ortho(0.0f, static_cast<GLfloat>(settings->screenResolution.x), 0.0f,
        static_cast<GLfloat>(settings->screenResolution.y));
}

State::~State()
{
    for (auto model : deffemModelContexts)
    {
        delete model;
    }
    deffemModelContexts.clear();
    delete heatmap;
}


bool State::animationTick()
{
    if (animation.play)
    {
        const auto currentTimeMillis = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        );

        if (currentTimeMillis >= animation.nextDeadline)
        {
            const auto hasNext = nextModel();
            animation.isFinished = !hasNext;
            animation.play = hasNext;
            animation.nextDeadline = currentTimeMillis + animation.tickMillis;

            if(!hasNext && animation.repeat)
            {
                restartAnimation();
            }
        }

        return true;
    }

    return false;
}

void State::restartAnimation()
{
    if (!deffemModelContexts.empty())
    {
        changeModel(0);
        animation.start();
    }
}


bool State::nextModel()
{
    return changeModel(currentModelIndex + 1);
}

bool State::previousModel()
{
    return changeModel(currentModelIndex - 1);
}


bool State::changeModel(size_t index)
{
    if (index >= 0 && index < deffemModelContexts.size()) {
        currentModelIndex = index;
        delete heatmap;
        heatmap = new Heatmap(glm::fvec2(25.0f, screenSize.y / 2),
            settings->heatmapSize,
            deffemModelContexts[index]->info,
            settings->textColor);

        heatmap->setProjection(&HUDprojection);

        currentModelIndex = index;

        return true;
    }

    return false;
}

void State::clearModels()
{
    if (!deffemModelContexts.empty())
    {
        for (auto ctx : deffemModelContexts)
            delete ctx;

        deffemModelContexts.clear();
        refresh();
    }
}

void State::refresh()
{
    currentModelIndex = 0;
    const auto modelCtx = currentModelContext();

    if (modelCtx)
    {
        heatmap = new Heatmap(glm::fvec2(25.0f, screenSize.y / 2),
                              settings->heatmapSize,
                              deffemModelContexts[0]->info,
                              settings->textColor);

        heatmap->setProjection(&HUDprojection);
    }
    else
    {
        delete heatmap;
    }
}

ModelContext* State::currentModelContext()
{
    if (!deffemModelContexts.empty())
    {
        return deffemModelContexts[currentModelIndex];
    }

    return nullptr;
}

void State::displayStateInfoText(Shader* textShader, Typer* typer)
{
    const auto textColor = settings->textColor;
    const auto modelCtx = this->currentModelContext();

    if (modelCtx)
    {
        const auto scaleMsg = "Scale: " + std::to_string(modelScale);
        const auto stepMsg = "Step: " + std::to_string(currentModelIndex + 1) + " of " + std::to_string(
            deffemModelContexts.size()) + " loaded";
        const auto filenameMsg = "File: " + modelCtx->filename;

        typer->renderText(textShader, scaleMsg, glm::fvec2(10.0f, 60.0f), 0.3f, textColor);
        typer->renderText(textShader, stepMsg, glm::fvec2(10.0f, 30.0f), 0.3f, textColor);
        typer->renderText(textShader, filenameMsg, glm::fvec2(10.0f, 10.0f), 0.3f, textColor);
    }
    else
    {
        typer->renderText(textShader, "Drag and drop files here...",
                          glm::fvec2(screenSize.x / 2 - 100.0,
                                     screenSize.y / 2 - 5.0),
                          0.4f, textColor);
    }
}
