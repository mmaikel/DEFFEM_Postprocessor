#ifndef LINE_H
#define  LINE_H
#include "Object.h"


namespace deffem {
    class Line : Object
    {
    public:
        Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 color);
        // ~Line();
        void draw() override;
        void draw(Shader* shader) override;
    };
}
#endif
