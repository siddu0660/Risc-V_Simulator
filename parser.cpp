#include <bits/stdc++.h>
#include <vector>
#include "functions.h"

using namespace std;
using namespace simulator;

vector<pair<int64_t, int>> parseValues(const string &line, int baseSize, ofstream &errorFile, int lineNum)
{
    vector<pair<int64_t, int>> values;
    stringstream ss(line);
    string number;

    while (getline(ss, number, ','))
    {
        number.erase(0, number.find_first_not_of(" \t"));
        number.erase(number.find_last_not_of(" \t") + 1);

        if (number.empty())
        {
            continue;
        }

        int64_t value;
        if (number.find("0x") == 0 || number.find("0X") == 0)
        {
            value = stoll(number, nullptr, 16);
        }
        else
        {
            value = stoll(number);
        }

        if (baseSize == 8)
        {
            if (value < INT64_MIN || value > INT64_MAX)
            {
                errorFile << "Error: Value out of bounds on line " << lineNum << endl;
                exit(1);
            }
        }
        else if (baseSize == 4)
        {
            if (value < INT32_MIN || value > INT32_MAX)
            {
                errorFile << "Error: Value out of bounds on line " << lineNum << endl;
                exit(1);
            }
        }
        else if (baseSize == 2)
        {
            if (value < INT16_MIN || value > INT16_MAX)
            {
                errorFile << "Error: Value out of bounds on line " << lineNum << endl;
                exit(1);
            }
        }
        else if (baseSize == 1)
        {
            if (value < INT8_MIN || value > INT8_MAX)
            {
                errorFile << "Error: Value out of bounds on line " << lineNum << endl;
                exit(1);
            }
        }

        values.push_back(make_pair(value, baseSize));
    }

    return values;
}

vector<pair<int64_t, int>> extractData(ifstream &infile, string &line, ofstream &errorFile, int &lineNum)
{
    vector<pair<int64_t, int>> data;
    string lastDirective = "";
    int baseSize = 0;

    while (getline(infile, line))
    {
        line.erase(0, line.find_first_not_of(" \t\n"));
        line.erase(line.find_last_not_of(" \t\n") + 1);

        if (line.empty())
        {
            continue;
        }

        lineNum++;

        if (line == ".text")
        {
            return data;
        }

        size_t pos = string::npos;

        if ((pos = line.find(".dword")) != string::npos)
        {
            lastDirective = ".dword";
            baseSize = 8;
            string values = line.substr(pos + 6);
            vector<pair<int64_t, int>> dwordValues = parseValues(values, baseSize, errorFile, lineNum);
            data.insert(data.end(), dwordValues.begin(), dwordValues.end());
        }
        else if ((pos = line.find(".word")) != string::npos)
        {
            lastDirective = ".word";
            baseSize = 4;
            string values = line.substr(pos + 5);
            vector<pair<int64_t, int>> wordValues = parseValues(values, baseSize, errorFile, lineNum);
            data.insert(data.end(), wordValues.begin(), wordValues.end());
        }
        else if ((pos = line.find(".half")) != string::npos)
        {
            lastDirective = ".half";
            baseSize = 2;
            string values = line.substr(pos + 9);
            vector<pair<int64_t, int>> halfwordValues = parseValues(values, baseSize, errorFile, lineNum);
            data.insert(data.end(), halfwordValues.begin(), halfwordValues.end());
        }
        else if ((pos = line.find(".byte")) != string::npos)
        {
            lastDirective = ".byte";
            baseSize = 1;
            string values = line.substr(pos + 5);
            vector<pair<int64_t, int>> byteValues = parseValues(values, baseSize, errorFile, lineNum);
            data.insert(data.end(), byteValues.begin(), byteValues.end());
        }
        else
        {
            if (!lastDirective.empty())
            {
                vector<pair<int64_t, int>> values = parseValues(line, baseSize, errorFile, lineNum);
                data.insert(data.end(), values.begin(), values.end());
            }
            else
            {
                errorFile << "Error: Unrecognized directive or missing previous directive on line " << lineNum << endl;
            }
        }
    }

    return data;
}

vector<string> extractInstructions(ifstream &infile, unordered_map<string, int> &labelMap, ofstream &errorFile, int &lineNum)
{
    string line;
    vector<string> instructions;

    while (getline(infile, line))
    {
        line.erase(0, line.find_first_not_of(" \t\n"));
        line.erase(line.find_last_not_of(" \t\n") + 1);

        if (line.empty())
        {
            continue;
        }

        lineNum++;

        size_t colon = line.find(':');
        if (colon != string::npos)
        {
            string label = line.substr(0, colon);
            label.erase(label.find_last_not_of(" \t") + 1);

            if (label.empty())
            {
                errorFile << "Error: Empty label on line " << lineNum << endl;
                continue;
            }

            labelMap[label] = lineNum - 1;

            string labelInstruction = line.substr(colon + 1);
            labelInstruction.erase(0, labelInstruction.find_first_not_of(" \t"));

            if (!labelInstruction.empty())
            {
                instructions.push_back(labelInstruction);
            }
        }
        else
        {
            instructions.push_back(line);
        }
    }

    return instructions;
}

void processFile(const string &filename, unordered_map<string, int> &labelMap, vector<pair<int64_t, int>> &data, vector<string> &instructions, const string &errorLog)
{
    ifstream infile(filename);
    ofstream errorFile(errorLog);
    string line;
    int lineNum = 0;
    bool dataSection = false;
    bool textSection = false;

    if (!infile.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    while (getline(infile, line))
    {
        line.erase(0, line.find_first_not_of(" \t\n"));
        line.erase(line.find_last_not_of(" \t\n") + 1);

        if (line.empty())
        {
            continue;
        }

        lineNum++;

        size_t colon = line.find(':');
        if (colon != string::npos)
        {
            string label = line.substr(0, colon);
            label.erase(label.find_last_not_of(" \t") + 1);

            if (label.empty())
            {
                errorFile << "Error: Empty label on line " << lineNum << endl;
                continue;
            }

            labelMap[label] = lineNum;

            string labelInstruction = line.substr(colon + 1);
            labelInstruction.erase(0, labelInstruction.find_first_not_of(" \t"));

            if (!labelInstruction.empty())
            {
                instructions.push_back(labelInstruction);
            }
        }
        else if (line == ".data")
        {
            dataSection = true;
            textSection = false;
            data = extractData(infile, line, errorFile, lineNum);
        }
        else if (line == ".text")
        {
            textSection = true;
            dataSection = false;
            instructions = extractInstructions(infile, labelMap, errorFile, lineNum);
        }
        else
        {
            if (!textSection && !dataSection)
            {
                textSection = true;
            }

            instructions.push_back(line);
        }
    }

    if (!dataSection && !textSection)
    {
        cerr << "Error: Neither .data nor .text section found in file." << endl;
        errorFile << "Error: Neither .data nor .text section found in file." << endl;
    }

    infile.close();
}

int simulator::parseMain(string filename, vector<string> &instructions)
{
    initializeMem();
    unordered_map<string, int> labelMap;
    vector<pair<int64_t, int>> data;
    vector<string> encodedInstructions;

    processFile(filename, labelMap, data, instructions, "error.log");
    setData(data);
    encodedInstructions = processInstructions(instructions, labelMap, 0, "error.log");
    setText(encodedInstructions);

    return 0;
}
