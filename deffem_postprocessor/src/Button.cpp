#include "../headers/Button.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

using namespace deffem;

Button::Button(glm::fvec2 pos, glm::fvec2 size, std::string texturePath): Rectangle(0.0, 0.0, 0.0f, size.x, size.y)
{
    this->position = pos;
    this->size = size;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
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

Button::~Button()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Button::draw()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    deffem::Rectangle::draw();
}

void Button::draw(Shader* shader)
{
    const auto translate = glm::translate(*projection, glm::fvec3(position.x, position.y, 0.0));
    
    shader->use();
    shader->setMat4("projection", translate);

    glBindTexture(GL_TEXTURE_2D, texture);
    deffem::Rectangle::draw();
}

// Sets new window size
void Button::setProjection(glm::fmat4* projection)
{
    this->projection = projection;
}

// Sets new position
void Button::setPosition(glm::fvec2 pos)
{
    position = pos;
}


bool Button::isClicked(glm::fvec2 pos)
{
    const auto inX = pos.x >= this->position.x && pos.x <= this->position.x + this->size.x;
    const auto inY = pos.y >= this->position.y && pos.y <= this->position.y + this->size.y;

    return inX && inY;
}

void Button::changeTexture(const std::string& texturePath)
{
    glDeleteTextures(1, &texture);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
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

