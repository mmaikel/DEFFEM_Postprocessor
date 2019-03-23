#include "Rectangle.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"

class Button : protected Rectangle
{
public:

    Button(glm::vec3 pos, glm::vec2 size, std::string texturePath)
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    ~Button()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw() override
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6 * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void draw(Shader *shader) override
    {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6 * sizeof(GLfloat), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

private:


};
