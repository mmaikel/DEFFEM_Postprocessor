#pragma once
#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H
#include "../headers/Color.h"
#include <map>
#include <glm/glm/detail/type_vec2.hpp>

class 

enum ThemeVariant
{
    light = 0,
    dark = 1
};

class ApplicationSettings
{
public:

    glm::vec2 screenResolution;

    ThemeVariant themeVariant;

    Color backgroundColor;

    bool showMeshPlane;

    float initialModelScale;

    long animationTickMillis;

    Color textColor() const
    {
        switch (themeVariant)
        {
        case ThemeVariant::light:
            return {0.0, 0.0, 0.0};

        case ThemeVariant::dark:
            return {1.0, 1.0, 1.0};

        default:
            return {0.5, 0.5, 0.5};
        }
    }

    ApplicationSettings(std::map<std::string, std::string> config);

};

#endif
