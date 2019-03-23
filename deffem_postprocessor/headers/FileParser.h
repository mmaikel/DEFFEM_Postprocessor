#pragma once
#ifndef FILE_PARSER_H
#define FILE_PARSER_H
#include <vector>
#include "ModelInfo.h"
#include <map>
#include <list>

namespace deffem
{
    class FileParser
    {
    public:

        static ModelInfo readSections(const const std::string& filename, std::vector<float>& verticesAndColors,
            std::vector<unsigned int>& indices);

        static std::map<std::string, std::string> readConfig(std::string filename);

    private:

        static unsigned long processLine(const std::string& line, std::vector<float>& vertices, std::list<float>& values,
            const char separator = ',');

        static unsigned long processLine(const std::string& line, std::vector<unsigned int>& indices, const char separator = ',');

        static bool checkSectionChanged(std::string test, std::string& section);

        static void getHeatMapColor(float value, float* red, float* green, float* blue);
    };
}

#endif
