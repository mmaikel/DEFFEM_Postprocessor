#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "enumerate.cpp"

using namespace std;

class FileParser
{
public:

    static int readSections(const char* filename, vector<float>& verticesAndColors, vector<unsigned int>& indices, float &min, float &max)
    {
        ifstream file;
        string line;
        string section;
        vector<float> values;
        vector<float> vertices;
        MinMaxValue minMax{};
        auto initial = true;

        file.open(filename);

        if (file.is_open())
        {
            cout << "[INFO]\tReading from file \"" << filename << "\"" << endl;
        }
        else
        {
            cout << "[ERROR]\tFailed to open file \"" << filename << "\"" << endl;
        }

        // Read from result file's sections into vectors: `vertices`, `values`, `indices`
        while (file.is_open() && !file.eof() && getline(file, line))
        {
            if (checkSectionChanged(line, section))
            {
                if (section == "*NODE") continue;
                if (section == "*END") break;
            }
            else if (section == "*NODE")
            {
                processLine(line, vertices, values);
                getMinMaxValue(line, minMax, initial);
            }
            else if (section == "*ELEMENT_SOLID")
            {
                processLine(line, indices);
            }
            else
            {
                std::stringstream ss;
                ss << "ERROR::FileParser::ReadSections\nUnknown section " << section <<
                    " found in results file " << filename << endl;
                throw std::runtime_error(ss.str());
            }
        }
        
        // Compose a vector with following structure: x1, y1, z1, r1, g1, b1, x2, y2, z2, r2, g2, b2, ... 
        for (auto val : deffem::enumerate(values))
        {
            const float normalizedVal = (val.item - minMax.min) / (minMax.max - minMax.min);
            const unsigned int insertIdx = val.index * 3;
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

        cout << "[INFO]\tFile \"" << filename << "\" was read successfully" << endl;
        cout << "[INFO]\tNumber of vertices: " << vertices.size() << endl;
        cout << "[INFO]\tNumber of indices: " << indices.size() << endl;
        cout << "[INFO]\tMinimum value: " << minMax.min << endl;
        cout << "[INFO]\tMaximum value: " << minMax.max << endl;


        vertices.clear();
        vertices.shrink_to_fit();
        values.clear();
        values.shrink_to_fit();

        min = minMax.min;
        max = minMax.max;

        file.close();
        return 0;
    }


private:

    struct MinMaxValue
    {
        float min;
        float max;
    };

    static void getMinMaxValue(const string& line, MinMaxValue& minMax, bool& initial)
    {
        std::stringstream ss(line);
        string element;
        string nodeNumber;
        auto idx = 0;

        while (getline(ss, element, ','))
        {
            if (idx == 4)
            {
                const auto value = strtof(element.c_str(), nullptr);
                if (initial)
                {
                    minMax.min = value;
                    minMax.max = value;
                    initial = false;
                }
                else
                {
                    if (value < minMax.min) minMax.min = value;
                    else if (value > minMax.max) minMax.max = value;
                }
            }

            idx++;
        }
    }

    static void processLine(const string& line, vector<float>& vertices, vector<float>& values)
    {
        // TODO: check in documentation if `line` is copied or referenced by {stringstream}
        std::stringstream ss(line);
        string element;
        string nodeNumber;
        auto idx = 0;

        getline(ss, nodeNumber, ',');

        while (getline(ss, element, ','))
        {
            if (idx < 3)
                vertices.push_back(strtof(element.c_str(), nullptr));
            else
                values.push_back(strtof(element.c_str(), nullptr));
            idx++;
        }
    }

    static void processLine(const string& line, vector<unsigned int>& indices)
    {
        std::stringstream ss(line);
        string element;
        string connectionNumber;
        string sectionId;

        getline(ss, connectionNumber, ',');
        getline(ss, sectionId, ',');

        float indicesHelper[8];
        int idx = 0;


        while (idx < 8 && getline(ss, element, ','))
        {
            indicesHelper[idx] = stoi(element) - 1;
            idx++;
        }

        if (idx > 8)
        {
            cout << "[ERROR]\tWrong input file format: There must be 8 node numbers in every row";
        }

        // TODO: Create convenient method for this:
        indices.push_back(indicesHelper[0]);
        indices.push_back(indicesHelper[1]);
        indices.push_back(indicesHelper[2]);
        indices.push_back(indicesHelper[0]);
        indices.push_back(indicesHelper[2]);
        indices.push_back(indicesHelper[3]);

     
        indices.push_back(indicesHelper[1]);
        indices.push_back(indicesHelper[5]);
        indices.push_back(indicesHelper[6]);
        indices.push_back(indicesHelper[1]);
        indices.push_back(indicesHelper[6]);
        indices.push_back(indicesHelper[2]);

        indices.push_back(indicesHelper[2]);
        indices.push_back(indicesHelper[6]);
        indices.push_back(indicesHelper[7]);
        indices.push_back(indicesHelper[2]);
        indices.push_back(indicesHelper[7]);
        indices.push_back(indicesHelper[3]);

        indices.push_back(indicesHelper[0]);
        indices.push_back(indicesHelper[4]);
        indices.push_back(indicesHelper[7]);
        indices.push_back(indicesHelper[0]);
        indices.push_back(indicesHelper[7]);
        indices.push_back(indicesHelper[3]);

        indices.push_back(indicesHelper[1]);
        indices.push_back(indicesHelper[5]);
        indices.push_back(indicesHelper[4]);
        indices.push_back(indicesHelper[1]);
        indices.push_back(indicesHelper[4]);
        indices.push_back(indicesHelper[0]);

        indices.push_back(indicesHelper[4]);
        indices.push_back(indicesHelper[5]);
        indices.push_back(indicesHelper[6]);
        indices.push_back(indicesHelper[4]);
        indices.push_back(indicesHelper[6]);
        indices.push_back(indicesHelper[7]);
    }

    static bool checkSectionChanged(string test, string& section)
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

    static void getHeatMapColor(float value, float* red, float* green, float* blue)
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
};
