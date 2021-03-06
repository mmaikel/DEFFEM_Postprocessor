#ifndef LINE_H
#define  LINE_H
#include "Object.h"
#include "Color.h"


namespace deffem
{
    class Line : Object
    {
    public:

        Line(glm::vec3 p1, glm::vec3 p2, Color color);
        
        void draw() override;

        void draw(Shader* shader) override;

        void draw(Shader* shader) const;

        void destroy() override;

    };
}
#endif
