#include "../headers/CustomObject.h"
#include <vector>
#include <glad/glad.h>

using namespace deffem;

CustomObject::CustomObject(std::vector<GLfloat> vertices, std::vector<GLuint> indices,
                           std::vector<GLuint> attributes, const int stride)
{
    this->verticesSize_ = vertices.size();
    this->indicesSize_ = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(),
                 GL_STATIC_DRAW);


    auto attrPtr = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                              reinterpret_cast<void*>(attrPtr * sizeof(GLfloat)));
        glEnableVertexAttribArray(i);
        attrPtr += attributes[i];
    }
}

CustomObject::CustomObject(std::vector<GLfloat> vertices, std::vector<GLuint> attributes, const int stride)
{
    this->indicesSize_ = 0;
    this->verticesSize_ = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    auto attrPtr = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                              reinterpret_cast<void*>(attrPtr * sizeof(GLfloat)));
        glEnableVertexAttribArray(i);
        attrPtr += attributes[i];
    }
}

void CustomObject::draw()
{
    glBindVertexArray(VAO);
    if (indicesSize_ == 0)
    {
        glDrawArrays(GL_POINTS, 0, verticesSize_);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indicesSize_ * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
    }
}

void CustomObject::draw(Shader* shader)
{
    shader->use();
    glBindVertexArray(VAO);
    if (indicesSize_ == 0)
    {
        glDrawArrays(GL_POINTS, 0, verticesSize_);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, indicesSize_ * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
    }
}

void CustomObject::destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


CustomObject::~CustomObject()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
