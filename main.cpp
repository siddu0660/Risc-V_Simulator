#include <bits/stdc++.h>
#include "functions.h"

using namespace std;
using namespace simulator;

void loadFile(const string &filename, vector<string> &instructions)
{
    parseMain(filename,instructions);
    cout << endl;
}
void run(uint32_t &PC, vector<string> &instructions, vector<int> &breakpoints)
{
    vector<uint32_t> encodedInstructions = fetchInst(0);
    executeInstruction(instructions, encodedInstructions, PC,false,breakpoints);
    cout << endl;
};

void showReg()
{
    printRegisters();
    cout << endl;
};
void exitSim()
{
    cout << "Simulation Terminated !!" << endl;
};
void showMem(uint32_t addr, int count)
{
    printMem(addr, count);
    cout << endl;
};
void stepInst(uint32_t &PC, vector<int> &breakpoints, vector<string> &instructions)
{
    vector<uint32_t> encodedInstructions = fetchInst(0);
    executeInstruction(instructions,encodedInstructions, PC, true, breakpoints);
    cout << endl;
};
void showStack()
{
    cout << "Show Stack....." << endl;
    cout << endl;
};
void setBreakpoint(int line,vector<int> &breakpoints)
{
    breakpoints.push_back(line);
    sort(breakpoints.begin(), breakpoints.end());
    cout << "Breakpoint set at line " << line << endl;
    cout << endl;
};
void deleteBreakpoint(int line,vector<int> &breakpoints)
{
    auto i = find(breakpoints.begin(), breakpoints.end(), line);
    if (i != breakpoints.end())
    {
        breakpoints.erase(i);
    }
    cout << "Breakpoint deleted at line " << line << endl;
    cout << endl;
};

int main()
{
    uint32_t PC = 0;
    vector<string> instructions;
    vector<int> breakpoints;
    string input;
    while (true)
    {
        getline(cin, input);
        stringstream ss(input);
        string command, arg1, arg2;
        ss >> command >> arg1 >> arg2;

        if(command == "test")
        {
            loadFile("input.s",instructions);
            run(PC, instructions, breakpoints);
        }
        if (command == "load")
        {
            PC = 0;
            loadFile(arg1, instructions);
        }
        else if (command == "run")
        {
            run(PC, instructions, breakpoints);
        }
        else if (command == "regs")
        {
            showReg();
        }
        else if (command == "exit")
        {
            exitSim();
            break;
        }
        else if (command == "mem")
        {
            uint32_t addr = stoul(arg1, nullptr, 16);
            int count = stoi(arg2);
            showMem(addr, count);
        }
        else if (command == "step")
        {
            stepInst(PC, breakpoints, instructions);
        }
        else if (command == "show-stack")
        {
            showStack();
        }
        else if (command == "break")
        {
            int line = stoul(arg1, nullptr, 10);
            setBreakpoint(line, breakpoints);
        }

        else if (command == "del" && arg1 == "break")
        {
            int line = stoul(arg2, nullptr, 10);
            deleteBreakpoint(line,breakpoints);
        }
        else
        {
            cout << "Invalid command!" << endl;
        }
    }

    return 0;
}