#include <bits/stdc++.h>
#include "functions.h"

using namespace std;
using namespace simulator;

void loadFile(const string &filename, vector<string> &instructions)
{
    parseMain(filename,instructions);
}
void run(uint32_t &PC, vector<string> &instructions)
{
    vector<uint32_t> encodedInstructions = fetchInst(0);
    executeInstruction(encodedInstructions, PC,false);
};

void showReg()
{
    printRegisters();
};
void exitSim()
{
    cout << "Simulation Terminated !!" << endl;
};
void showMem(uint32_t addr, int count)
{
    printMem(addr, count);
};
void stepInst(uint32_t &PC)
{
    vector<uint32_t> encodedInstructions = fetchInst(0);
    for(auto &inst : encodedInstructions)
    {
        cout << "0x" << hex << inst << endl;
    }
    executeInstruction(encodedInstructions, PC, true);
};
void showStack()
{
    cout << "Show Stack....." << endl;
};
void setBreakpoint(int line)
{
    cout << "Set Breakpoint....." << endl;
};
void deleteBreakpoint(int line)
{
    cout << "Delete Breakpoint....." << endl;
};

int main()
{
    uint32_t PC = 0;
    vector<string> instructions;
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
            run(PC, instructions);
        }
        if (command == "load")
        {
            PC = 0;
            loadFile(arg1, instructions);
        }
        else if (command == "run")
        {
            run(PC, instructions);
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
            stepInst(PC);
        }
        else if (command == "show-stack")
        {
            showStack();
        }
        else if (command == "break")
        {
            int line = stoi(arg1);
            setBreakpoint(line);
        }
        else if (command == "del" && arg1 == "break")
        {
            int line = stoi(arg2);
            deleteBreakpoint(line);
        }
        else
        {
            cout << "Invalid command!" << endl;
        }
    }

    return 0;
}