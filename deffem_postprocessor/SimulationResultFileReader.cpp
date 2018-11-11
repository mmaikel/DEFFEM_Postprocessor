#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class FileReader
{
public:

	static float* readVertices(const char* filename, size_t size)
	{
		ifstream file;
		string line;
		string section;
		unsigned int idx = 0;
		float* vertices = new float[size]();

		file.open(filename);

		while (file.is_open() && !file.eof() && getline(file, line))
		{
			// Remove whitespace
			// cell.erase(remove_if(cell.begin(), cell.end(), isspace), cell.end());

			if (checkSectionChanged(line, section))
			{
				// TODO: Implement behaviour after section change
			}
			else if (section == "*NODE")
			{
				processLine(line, vertices, idx);
			}
			else
			{
				break;
			}
		}

		file.close();
		return vertices;
	}

	static unsigned int* readIndices(const char* filename, size_t size)
	{
		ifstream file;
		string line;
		string section;
		unsigned int idx = 0;
		unsigned int* indices = new unsigned int[size]();

		file.open(filename);

		while (file.is_open() && !file.eof() && getline(file, line))
		{
			// Remove whitespace
			// cell.erase(remove_if(cell.begin(), cell.end(), isspace), cell.end());

			if (checkSectionChanged(line, section))
			{
				// TODO: Implement behaviour after section change
			}
			else if (section == "*ELEMENT_SOLID")
			{
				processLine(line, indices, idx);
			}
		}

		file.close();
		return indices;
	}

	static void processLine(string line, float* vertices, unsigned int& idx)
	{
		std::stringstream ss(line);
		string element;

		getline(ss, element, ',');

		while (getline(ss, element, ','))
		{
			const float f = strtod(element.c_str(), nullptr);
			// cout << f << endl;
			// TODO: Return proper array of vertices
			vertices[idx] = f;

			idx++;
		}
	}

	static void processLine(string line, unsigned int* indices, unsigned int& idx)
	{
		std::stringstream ss(line);
		string element;

		getline(ss, element, ',');

		while (getline(ss, element, ','))
		{
			const float f = strtod(element.c_str(), nullptr);
			// cout << f << endl;
			// TODO: Return proper array of indices
			indices[idx] = f;

			idx++;
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
