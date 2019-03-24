#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "enumerate.cpp"
#include <list>
#include <algorithm>
#include <math.h>
#include <iterator>
#include <map>
#include "../headers/FileParser.h"


using namespace std;
using namespace deffem;

ModelInfo FileParser::readSections(const string& filename, vector<GLfloat>& verticesAndColors,
                                   vector<GLuint>& indices)
{
    ifstream file;
    string currentLine;
    string currentSection;
    list<GLfloat> values;
    vector<float> vertices;
    unsigned long lastNodeNumber = 0;
    unsigned long lastElementNumber = 0;

    file.open(filename);

    if (file.is_open())
    {
        cout << "[INFO]\tReading from file \"" << filename << "\"" << endl;
    }
    else
    {
        cout << "[ERROR]\tFailed to open file \"" << filename << "\"" << endl;
    }

    // Read from result file's sections into vectors: `vertices`, `heatMapRows`, `indices`
    while (file.is_open() && !file.eof() && getline(file, currentLine))
    {
        if (checkSectionChanged(currentLine, currentSection))
        {
            if (currentSection == "*NODE") continue;
            if (currentSection == "*END") break;
        }
        else if (currentSection.empty())
        {
            lastNodeNumber = processLine(currentLine, vertices, values, ' ');
        }
        else if (currentSection == "*NODE")
        {
            lastNodeNumber = processLine(currentLine, vertices, values);
        }
        else if (currentSection == "*ELEMENT_SOLID")
        {
            lastElementNumber = processLine(currentLine, indices);
        }
        else
        {
            std::stringstream ss;
            ss << "ERROR::FileParser::ReadSections\nUnknown section " << currentSection <<
                " found in results file " << filename << endl;
            throw std::runtime_error(ss.str());
        }
    }

    const auto maxValue = *max_element(std::begin(values), std::end(values));
    const auto minValue = *min_element(std::begin(values), std::end(values));

    // Compose a vector with following structure: x1, y1, z1, r1, g1, b1, x2, y2, z2, r2, g2, b2, ... 
    for (auto val : deffem::enumerate(values))
    {
        const auto normalizedVal = (val.item - minValue) / (maxValue - minValue);
        const auto insertIdx = val.index * 3;
        float r, g, b;

        getHeatMapColor(normalizedVal, &r, &g, &b);

        if (insertIdx < vertices.size())
        {
            verticesAndColors.push_back(vertices[insertIdx]);
            verticesAndColors.push_back(vertices[insertIdx + 1]);
            verticesAndColors.push_back(vertices[insertIdx + 2]);
        }

        verticesAndColors.push_back(r);
        verticesAndColors.push_back(g);
        verticesAndColors.push_back(b);
    }

    const auto modelInfo = ModelInfo{lastNodeNumber, lastElementNumber, MinMax(minValue, maxValue)};


    vertices.clear();
    vertices.shrink_to_fit();
    values.clear();
    file.close();

    return modelInfo;
}

std::map<string, string> FileParser::readConfig(string filename)
{
    ifstream file;
    auto map = std::map<string, string>();

    file.open(filename);

    if (file.is_open())
    {
        cout << "[INFO]\tReading from file \"" << filename << "\"" << endl;
    }
    else
    {
        cout << "[ERROR]\tFailed to open file \"" << filename << "\"" << endl;
    }


    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string key;
        if (std::getline(ss, key, '='))
        {
            std::string value;
            if (std::getline(ss, value))
            {
                cout << key << " = " << value << endl;
                map.insert(std::pair<string, string>(key, value));
            }
        }
    }

    return map;
}


unsigned long FileParser::processLine(const string& line, vector<GLfloat>& vertices, list<GLfloat>& values,
                                      const char separator)
{
    std::stringstream ss(line);
    string element;
    string nodeNumber;
    auto idx = 0;

    vector<float> value;


    while (getline(ss, element, separator))
    {
        if (!element.empty())
        {
            nodeNumber = element;
            break;
        }
    }

    while (getline(ss, element, separator))
    {
        if (!element.empty())
        {
            if (idx < 3)
                vertices.push_back(strtof(element.c_str(), nullptr));
            else
                value.push_back(strtof(element.c_str(), nullptr));
            idx++;
        }
    }

    if (value.size() == 1)
    {
        values.push_back(value.back());
    }
    else if (value.size() == 3)
    {
        auto x = value[0];
        auto y = value[1];
        auto z = value[2];

        auto mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

        values.push_back(mag);
    }


    return stoul(nodeNumber);
}

unsigned long FileParser::processLine(const string& line, vector<GLuint>& indices, const char separator)
{
    std::stringstream ss(line);
    string element;
    string connectionNumber;
    string sectionId;

    getline(ss, connectionNumber, separator);
    getline(ss, sectionId, separator);

    GLuint _indices[8];
    auto idx = 0;

    while (idx < 8 && getline(ss, element, separator))
    {
        if (!element.empty())
        {
            _indices[idx] = stoi(element) - 1;
            idx++;
        }
    }

    if (idx > 8)
    {
        cout << "[ERROR]\tWrong input file format: There must be 8 node numbers in every row";
    }

    // Builds cube with triangles
    indices.push_back(_indices[0]);
    indices.push_back(_indices[1]);
    indices.push_back(_indices[2]);
    indices.push_back(_indices[0]);
    indices.push_back(_indices[2]);
    indices.push_back(_indices[3]);

    indices.push_back(_indices[1]);
    indices.push_back(_indices[5]);
    indices.push_back(_indices[6]);
    indices.push_back(_indices[1]);
    indices.push_back(_indices[6]);
    indices.push_back(_indices[2]);

    indices.push_back(_indices[2]);
    indices.push_back(_indices[6]);
    indices.push_back(_indices[7]);
    indices.push_back(_indices[2]);
    indices.push_back(_indices[7]);
    indices.push_back(_indices[3]);

    indices.push_back(_indices[0]);
    indices.push_back(_indices[4]);
    indices.push_back(_indices[7]);
    indices.push_back(_indices[0]);
    indices.push_back(_indices[7]);
    indices.push_back(_indices[3]);

    indices.push_back(_indices[1]);
    indices.push_back(_indices[5]);
    indices.push_back(_indices[4]);
    indices.push_back(_indices[1]);
    indices.push_back(_indices[4]);
    indices.push_back(_indices[0]);

    indices.push_back(_indices[4]);
    indices.push_back(_indices[5]);
    indices.push_back(_indices[6]);
    indices.push_back(_indices[4]);
    indices.push_back(_indices[6]);
    indices.push_back(_indices[7]);

    return stoul(connectionNumber);
}

bool FileParser::checkSectionChanged(string test, string& section)
{
    if (test[0] == '*')
    {
        if (!(*&section).empty())
            cout << "[DEBUG]\tFinished reading section " << section << endl;

        section = test;

        return true;
    }

    return false;
}

void FileParser::getHeatMapColor(float value, float* red, float* green, float* blue)
{
    const int NUM_COLORS = 4;
    static float color[NUM_COLORS][3] = {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1; // |-- Our desired color will be between these two indexes in "color".
    int idx2; // |
    float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

    if (value <= 0) { idx1 = idx2 = 0; } // accounts for an input <=0
    else if (value >= 1) { idx1 = idx2 = NUM_COLORS - 1; } // accounts for an input >=0
    else
    {
        value = value * (NUM_COLORS - 1); // Will multiply value by 3.
        idx1 = floor(value); // Our desired color will be after this index.
        idx2 = idx1 + 1; // ... and before this index (inclusive).
        fractBetween = value - float(idx1); // Distance between the two indexes (0-1).
    }

    *red = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
    *green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
    *blue = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
}
