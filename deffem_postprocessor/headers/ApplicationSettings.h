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

        Color meshPlaneColor;

        glm::fvec2 heatmapSize;

        bool showMeshPlane;

        float initialModelScale;

        long animationTickMillis;

        int lineWidth;

        int pointSize;

        ApplicationSettings();

        explicit ApplicationSettings(std::map<std::string, std::string> config);

    private:

        Color textColorFn() const
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

        Color meshPlaneColorFn() const
        {
            switch (themeVariant)
            {
            case ThemeVariant::light:
                return {0.8, 0.8, 0.8};

            case ThemeVariant::dark:
                return {0.1, 0.1, 0.1};

            default:
                return {0.5, 0.5, 0.5};
            }
        }
    };
}

#endif
