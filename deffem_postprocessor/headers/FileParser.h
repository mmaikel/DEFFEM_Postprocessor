#pragma once
#ifndef FILE_PARSER_H
#define FILE_PARSER_H
#include <vector>
#include "ModelInfo.h"
#include <map>
#include <list>
#include <glad/glad.h>

namespace deffem
{
    class FileParser
    {
    public:

        static ModelInfo readSections(const std::string& filename, std::vector<GLfloat>& verticesAndColors,
                                      std::vector<GLuint>& indices);

        static std::map<std::string, std::string> readConfig(std::string filename);

    private:

        static unsigned long processLine(const std::string& line, std::vector<GLfloat>& vertices,
                                         std::list<GLfloat>& values,
                                         char separator = ',');

        static unsigned long processLine(const std::string& line, std::vector<GLuint>& indices, char separator = ',');

        static bool checkSectionChanged(std::string test, std::string& section);

        static void getHeatMapColor(float value, float* red, float* green, float* blue);
    };
}

#endif
