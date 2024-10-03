#include <bits/stdc++.h>
using namespace std;

namespace simulator
{
    void setData(vector<pair<int64_t, int>> data);
    void setText(vector<string> encodedInstructions);
    vector<string> processInstructions(const vector<string> &instructions, unordered_map<string, int> &labelMap, int PC, const string &errorLog);
    void initializeMem();
    int parseMain(string filename, vector<string> instructions);
    void printMem(uint32_t addr, int n);
    void printRegisters();
    void writeReg(uint32_t regName, int64_t value);
    uint64_t readReg(uint32_t regName);
    void writeMem(uint32_t addr, uint64_t value, size_t size);
    uint64_t readMem(uint32_t addr, size_t size);
    vector<uint32_t> fetchInst(uint32_t PC);
    void executeInstruction(vector<uint32_t> instructions, uint32_t &PC, bool step);
}