#include "../headers/Object.h"
#include <vector>

namespace deffem
{
    class CustomObject : protected Object
    {
    public:

        CustomObject()
        {
            
        }

        CustomObject(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> attribs, int stride)
        {
            this->indicesSize = indices.size();

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                         GL_STATIC_DRAW);


            int attrPtr = 0;
            for (auto i = 0; i < attribs.size(); i++)
            {        
                glVertexAttribPointer(i, attribs[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), reinterpret_cast<void*>(attrPtr * sizeof(float)));
                glEnableVertexAttribArray(i);
                attrPtr += attribs[i];
            }

            // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        }

        void draw() override
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr);
        }

        void draw(Shader shader) override
        {
            shader.use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indicesSize * sizeof(float), GL_UNSIGNED_INT, nullptr);
        }

    private:
        int indicesSize;
    };
}
