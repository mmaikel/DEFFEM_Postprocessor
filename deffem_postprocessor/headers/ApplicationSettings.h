#pragma once
#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include "../headers/Color.h"
#include <map>
#include <glm/glm/glm.hpp>

namespace deffem
{
    class ThemeVariant
    {
    public:

        enum ThemeVariants
        {
            light = 0,
            dark = 1
        };

        static ThemeVariants determine(const Color color)
        {
            const auto isLight = (0.3 * color.red + 0.59 * color.green + 0.11 * color.blue) >= 0.5;

            if (isLight)
            {
                return light;
            }

            return dark;
        }
    };


    class ApplicationSettings
    {
    public:

        glm::vec2 screenResolution;
        ThemeVariant::ThemeVariants themeVariant;
        Color backgroundColor;
        Color textColor;
        Color gridPlaneColor;
        glm::fvec2 heatmapSize;
        bool showGridPlane;
        GLfloat modelScale;
        long animationTickMillis;
        GLfloat lineWidth;
        GLuint pointSize;
        GLfloat cameraRadius;
        glm::fvec3 modelIntersection;

        std::map<std::string, std::string> config;

        ApplicationSettings();

        explicit ApplicationSettings(std::map<std::string, std::string> config);

        void resetAll();

    private:

        Color textColorFn() const
        {
            switch (themeVariant)
            {
            case ThemeVariant::light:
                return {0.0f, 0.0f, 0.0f};

            case ThemeVariant::dark:
                return {1.0f, 1.0f, 1.0f};

            default:
                return {0.5f, 0.5f, 0.5f};
            }
        }

        Color meshPlaneColorFn() const
        {
            switch (themeVariant)
            {
            case ThemeVariant::light:
                return {backgroundColor.red - 0.15f, backgroundColor.green - 0.15f, backgroundColor.blue - 0.15f };

            case ThemeVariant::dark:
                return { backgroundColor.red + 0.15f, backgroundColor.green + 0.15f, backgroundColor.blue + 0.15f };

            default:
                return {0.5f, 0.5f, 0.5f};
            }
        }
    };
}

#endif
