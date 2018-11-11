#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class SimulationResultsFileReader
{
public:

	static int readNodeSection(const char* filename, vector<float>& vertices, vector<float>& values)
	{
		ifstream file;
		string line;
		string section;

		file.open(filename);

		while (file.is_open() && !file.eof() && getline(file, line))
		{
			if (checkSectionChanged(line, section) && section == "*ELEMENT_SOLID")
				break;
			else if (section == "*NODE")
			{
				processLine(line, vertices, values);
			}
		}

		file.close();
		return 0;
	}

	static int readElementSolidSection(const char* filename, vector<unsigned int>& indices)
	{
		ifstream file;
		string line;
		string section;

		file.open(filename);

		while (file.is_open() && !file.eof() && getline(file, line))
		{
			if (checkSectionChanged(line, section) && section == "*END")
				break;
			else if (section == "*ELEMENT_SOLID")
			{
				processLine(line, indices);
			}
		}

		file.close();
		return 0;
	}

private:

	static void processLine(const string& line, vector<float>& vertices, vector<float>& values)
	{
		std::stringstream ss(line);
		string element;
		string nodeNumber;

		getline(ss, nodeNumber, ',');

		auto idx = 0;

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

		getline(ss, element, ',');

		while (getline(ss, element, ','))
		{
			indices.push_back(stoi(element) - 1);
		}
	}

	static bool checkSectionChanged(string test, string& section)
	{
		if (test[0] == '*')
		{
			section = test;
			cout << "[INFO]\tSection changed to " << section << endl;
			return true;
		}

		return false;
	}
};
