#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class FileReader
{
public:
	static int readVertices(const char* filename)
	{
		ifstream infile;
		string cell;
		infile.open(filename);

		string section;


		while (infile.is_open() && !infile.eof() && getline(infile, cell))
		{
			// get a whole line
			std::stringstream ss(cell);

			if (!checkSectionChanged(cell, section))
			{
				while (getline(ss, cell, ','))
				{
					processBySection(cell, section);
				}
			}
		}


		infile.close();
		return 0;
	}

	static void processBySection(const string& cell, const string& section)
	{
		if (section == "*NODE")
		{
			const float f = strtod(cell.c_str(), nullptr);
			cout << f << endl;
		}
		else if (section == "*ELEMENT_SOLID")
		{
			const int i = stoi(cell);
			cout << i << endl;
		}
		else
		{
			cout << "[ERROR]\t" << section << " processing is not implemented" << endl;
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
