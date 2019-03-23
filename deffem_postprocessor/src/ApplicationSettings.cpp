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
    this->showMeshPlane = true;
    this->initialModelScale = 1.0;
    this->animationTickMillis = 50;
    this->lineWidth = 1;
    this->pointSize = 1;
    this->heatmapSize = glm::fvec2(50.0, 200.0);
}


ApplicationSettings::ApplicationSettings(std::map<std::string, std::string> config)
{
    stringstream ss(config["resolution"]);
    const auto backgroundColor = Color::fromString(config["background-color"]);
    string w, h;

    getline(ss, w, 'x');
    getline(ss, h);

    this->screenResolution = vec2(stof(w), stof(h));
    this->backgroundColor = backgroundColor;
    this->themeVariant = ThemeVariant::determine(backgroundColor);
    this->showMeshPlane = config["show-mesh"] == "true";
    this->initialModelScale = stof(config["model-scale"]);
    this->animationTickMillis = stol(config["animation-tick-millis"]);
    this->lineWidth = stoi(config["line-width"]);
    this->pointSize = stoi(config["point-size"]);
    this->textColor = textColorFn();
    this->meshPlaneColor = meshPlaneColorFn();
    this->heatmapSize = glm::fvec2(stof(config["heatmap-width"]), stof(config["heatmap-height"]));

}
