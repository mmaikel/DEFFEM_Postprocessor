#include "../headers/Line.h"


deffem::Line::Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 color)
{
    GLfloat vertices[] = {
        p1.x, p1.y, p1.z, color.x, color.y, color.z,
        p2.x, p2.y, p2.z, color.x, color.y, color.z
    };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);


    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

// deffem::Line::~Line()
// {
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);
// }


void deffem::Line::draw()
{
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void deffem::Line::draw(Shader* shader)
{
    shader->use();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, 6);
}
