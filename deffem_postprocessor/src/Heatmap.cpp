#include <vector>
#include "CustomObject.cpp"
#include <map>
#include <list>
#include "Rectangle.cpp"
#include "Typer.cpp"


class Heatmap : Object
{
public:

    Heatmap(GLfloat x, GLfloat y, GLfloat width, GLfloat height, float min, float max)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        int precision = 10;
        float heightGap = height / precision;
        float valueGap = (max - min) / precision;
        float red, green, blue;

        for (auto i = 0; i < precision*2; i++)
        {
            auto val = (i * valueGap) + min;
            const float normalizedVal = (val - min) / (max - min);
            getHeatMapColor(normalizedVal, &red, &green, &blue);

            vertices.push_back(x);
            vertices.push_back(y + (heightGap * i));
            vertices.push_back(0.0f);
            vertices.push_back(red);
            vertices.push_back(green);
            vertices.push_back(blue);

            vertices.push_back(x + width);
            vertices.push_back(y + (heightGap * i));
            vertices.push_back(0.0f);
            vertices.push_back(red);
            vertices.push_back(green);
            vertices.push_back(blue);

            if (i % 2 == 0)
            {
                indices.push_back(i);
                indices.push_back(i + 1);
                indices.push_back(i + 3);
            } else
            {
                indices.push_back(i + 1);
                indices.push_back(i + 2);
                indices.push_back(i - 1);
            }

            valPoints.push_back(x + width);
            valPoints.push_back(y + (heightGap * i));
            valPoints.push_back(0.0f);

            if (i <= 10) {
                vals.push_front(glm::vec4(x + width + 5.0f, y + (heightGap * i), 0.0f, val));
            }
        }

        std::vector<unsigned int> attribsSizes;
        attribsSizes.push_back(3);
        attribsSizes.push_back(3);

        


        this->heatmap = new deffem::CustomObject(vertices, indices, attribsSizes, 6);
    }

    void getHeatMapColor(float value, float* red, float* green, float* blue)
    {
        const int NUM_COLORS = 4;
        static float color[NUM_COLORS][3] = {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
        // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

        int idx1; // |-- Our desired color will be between these two indexes in "color".
        int idx2; // |
        float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

        if (value <= 0) { idx1 = idx2 = 0; } // accounts for an input <=0
        else if (value >= 1) { idx1 = idx2 = NUM_COLORS - 1; } // accounts for an input >=0
        else
        {
            value = value * (NUM_COLORS - 1); // Will multiply value by 3.
            idx1 = floor(value); // Our desired color will be after this index.
            idx2 = idx1 + 1; // ... and before this index (inclusive).
            fractBetween = value - float(idx1); // Distance between the two indexes (0-1).
        }

        *red = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
        *green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
        *blue = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
    }


    void draw() override
    {
        heatmap->draw();
    }

    void draw(Shader shader) override
    {
        heatmap->draw();
    }

    void draw(Shader shader, Shader tShader) 
    {
        heatmap->draw(shader);
        for(auto vec : vals)
        {
            deffem::Rectangle rec(vec.x, vec.y, vec.z, 15.0f, 1.0f, Color(1.0f, 1.0f, 1.0f));
            rec.draw(shader);
            typer.renderText(tShader, std::to_string(static_cast<int>(vec.w)), vec.x + 25.0f, vec.y, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }

    std::vector<float> valPoints;


protected:

    deffem::CustomObject* heatmap;

    std::list<glm::vec4> vals;

    Typer typer;


    
};
