#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm/mat4x4.hpp>

namespace deffem
{
    class Shader
    {
    public:
        // the program ID
        unsigned int ID;

        // constructor reads and builds the shader
        Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
        // use/activate the shader
        void use() const;

        // utility uniform functions
        void setBool(const std::string& name, bool value) const;

        void setInt(const std::string& name, int value) const;

        void setFloat(const std::string& name, float value) const;

        void setVec3(const GLchar* name, glm::vec3 value) const;

        void setMat4(const GLchar* name, glm::mat4 value) const;
    };
}

#endif
