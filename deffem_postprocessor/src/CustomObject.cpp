#include "../headers/CustomObject.h"
#include <vector>
#include <glad/glad.h>

using namespace deffem;

CustomObject::CustomObject(std::vector<float> vertices, std::vector<unsigned int> indices,
                           std::vector<unsigned int> attributes, const int stride)
{
    this->verticesSize = vertices.size();
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


    auto attrPtr = 0;
    for (auto i = 0; i < attributes.size(); i++)
    {
        glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              reinterpret_cast<void*>(attrPtr * sizeof(float)));
        glEnableVertexAttribArray(i);
        attrPtr += attributes[i];
    }
}

CustomObject::CustomObject(std::vector<float> vertices, std::vector<unsigned int> attributes, int stride)
{
    this->indicesSize = 0;
    this->verticesSize = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    int attrPtr = 0;
    for (auto i = 0; i < attributes.size(); i++)
    {
        glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              reinterpret_cast<void*>(attrPtr * sizeof(float)));
        glEnableVertexAttribArray(i);
        attrPtr += attributes[i];
    }
}

void CustomObject::draw()
{
    glBindVertexArray(VAO);
    if (indicesSize == 0)
    {
        glDrawArrays(GL_POINTS, 0, verticesSize);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indicesSize * sizeof(float), GL_UNSIGNED_INT, nullptr);
    }
}

void CustomObject::draw(Shader* shader)
{
    shader->use();
    glBindVertexArray(VAO);
    if (indicesSize == 0)
    {
        glDrawArrays(GL_POINTS, 0, verticesSize);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indicesSize * sizeof(float), GL_UNSIGNED_INT, nullptr);
    }
}

CustomObject::~CustomObject()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
