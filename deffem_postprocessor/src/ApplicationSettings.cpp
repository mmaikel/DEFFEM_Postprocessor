#include "../headers/ApplicationSettings.h"
#include <sstream>

using namespace std;
using namespace glm;
using namespace deffem;

ApplicationSettings::ApplicationSettings()
{
    this->screenResolution = vec2(1280, 720);
    this->backgroundColor = Color(1.0, 1.0, 1.0);
    this->themeVariant = ThemeVariant::light;
    this->showGridPlane = true;
    this->modelScale = 1.0;
    this->animationTickMillis = 50;
    this->lineWidth = 1;
    this->pointSize = 1;
    this->heatmapSize = fvec2(50.0, 200.0);
    this->modelIntersection = fvec3(1.0f, 1.0f, 1.0f);
    this->cameraRadius = 0.25f;
}


ApplicationSettings::ApplicationSettings(std::map<std::string, std::string> config)
{
    stringstream ss(config["resolution"]);
    const auto backgroundColor = Color::fromString(config["background-color"]);
    string w, h;

    getline(ss, w, 'x');
    getline(ss, h);

    this->config = config;
    this->screenResolution = vec2(stof(w), stof(h));
    this->backgroundColor = backgroundColor;
    this->themeVariant = ThemeVariant::determine(backgroundColor);
    this->showGridPlane = config["show-grid"] == "true";
    this->modelScale = stof(config["model-scale"]);
    this->animationTickMillis = stol(config["animation-tick-millis"]);
    this->lineWidth = stof(config["line-width"]);
    this->pointSize = stoi(config["point-size"]);
    this->textColor = textColorFn();
    this->gridPlaneColor = meshPlaneColorFn();
    this->heatmapSize = fvec2(stof(config["heatmap-width"]), stof(config["heatmap-height"]));
    this->cameraRadius = stof(config["camera-radius"]);
    this->modelIntersection = fvec3(stof(config["model-intersect-x"]),
                                    stof(config["model-intersect-y"]),
                                    stof(config["model-intersect-z"]));
}

void ApplicationSettings::resetAll()
{
    if (config.empty())
    {
        this->screenResolution = vec2(1280, 720);
        this->backgroundColor = Color(1.0, 1.0, 1.0);
        this->themeVariant = ThemeVariant::light;
        this->showGridPlane = true;
        this->modelScale = 1.0;
        this->animationTickMillis = 50;
        this->lineWidth = 1;
        this->pointSize = 1;
        this->heatmapSize = fvec2(50.0, 200.0);
        this->cameraRadius = 0.25f;
        this->modelIntersection = fvec3(1.0f, 1.0f, 1.0f);
    }
    else
    {
        stringstream ss(config["resolution"]);
        const auto backgroundColor = Color::fromString(config["background-color"]);
        string w, h;

        getline(ss, w, 'x');
        getline(ss, h);

        this->config = config;
        this->screenResolution = vec2(stof(w), stof(h));
        this->backgroundColor = backgroundColor;
        this->themeVariant = ThemeVariant::determine(backgroundColor);
        this->showGridPlane = config["show-grid"] == "true";
        this->modelScale = stof(config["model-scale"]);
        this->animationTickMillis = stol(config["animation-tick-millis"]);
        this->lineWidth = stoi(config["line-width"]);
        this->pointSize = stoi(config["point-size"]);
        this->textColor = textColorFn();
        this->gridPlaneColor = meshPlaneColorFn();
        this->heatmapSize = fvec2(stof(config["heatmap-width"]), stof(config["heatmap-height"]));
        this->cameraRadius = stof(config["camera-radius"]);
        this->modelIntersection = fvec3(stof(config["model-intersect-x"]),
                                        stof(config["model-intersect-y"]),
                                        stof(config["model-intersect-z"]));
    }
}
