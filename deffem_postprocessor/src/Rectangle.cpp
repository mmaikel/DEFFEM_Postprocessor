#include "../headers/Rectangle.h"


deffem::Rectangle::Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{

    GLfloat vertices[] = {
        x, y, z, 0.0f, 1.0f,
        x + width, y, z, 1.0f, 1.0f,
        x + width, y + height, z, 1.0f, 0.0f,
        x, y + height, z, 0.0f, 0.0f
    };

    GLint indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLfloat), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

deffem::Rectangle::Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, Color color)
{
    GLfloat vertices[] = {
        x, y, z, color.red, color.green, color.blue,
        x + width, y, z, color.red, color.green, color.blue,
        x + width, y + height, z, color.red, color.green, color.blue,
        x, y + height, z, color.red, color.green, color.blue
    };

    GLint indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLfloat), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

deffem::Rectangle::Rectangle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, Color color[4])
{
    GLfloat vertices[] = {
        x, y, z, color[0].red, color[0].green, color[0].blue,
        x + width, y, z, color[1].red, color[1].green, color[1].blue,
        x + width, y + height, z, color[2].red, color[2].green, color[2].blue,
        x, y + height, z, color[3].red, color[3].green, color[3].blue,
    };

    GLint indices[] = {0, 1, 2, 0, 2, 3};


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLfloat), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

deffem::Rectangle::~Rectangle()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void deffem::Rectangle::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


void deffem::Rectangle::draw(Shader* shader)
{
    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
