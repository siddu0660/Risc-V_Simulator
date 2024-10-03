#include <bits/stdc++.h>
#include <cstdint>
#include "functions.h"

using namespace std;
using namespace simulator;

#define registerCount 32
#define DATA_START 0x10000
#define TEXT_START 0x0
#define STACK_START 0x50000

uint32_t textTop = TEXT_START;
uint32_t dataTop = DATA_START;
uint32_t stackTop = STACK_START;

struct Register
{
    string name;
    string alias;
    int64_t value;
};

unordered_map<uint32_t, Register> registers = {
    {0, {"x0", "zero", 0x0}},
    {1, {"x1", "ra", 0x0}},
    {2, {"x2", "sp", stackTop}},
    {3, {"x3", "gp", 0x0}},
    {4, {"x4", "tp", 0x0}},
    {5, {"x5", "t0", 0x0}},
    {6, {"x6", "t1", 0x0}},
    {7, {"x7", "t2", 0x0}},
    {8, {"x8", "s0", 0x0}},
    {9, {"x9", "s1", 0x0}},
    {10, {"x10", "a0", 0x0}},
    {11, {"x11", "a1", 0x0}},
    {12, {"x12", "a2", 0x0}},
    {13, {"x13", "a3", 0x0}},
    {14, {"x14", "a4", 0x0}},
    {15, {"x15", "a5", 0x0}},
    {16, {"x16", "a6", 0x0}},
    {17, {"x17", "a7", 0x0}},
    {18, {"x18", "s2", 0x0}},
    {19, {"x19", "s3", 0x0}},
    {20, {"x20", "s4", 0x0}},
    {21, {"x21", "s5", 0x0}},
    {22, {"x22", "s6", 0x0}},
    {23, {"x23", "s7", 0x0}},
    {24, {"x24", "s8", 0x0}},
    {25, {"x25", "s9", 0x0}},
    {26, {"x26", "s10", 0x0}},
    {27, {"x27", "s11", 0x0}},
    {28, {"x28", "t3", 0x0}},
    {29, {"x29", "t4", 0x0}},
    {30, {"x30", "t5", 0x0}},
    {31, {"x31", "t6", 0x0}},
};
vector<uint8_t> textSegment;
vector<uint8_t> dataSegment;

void simulator::initializeMem()
{
    textSegment.resize(DATA_START - TEXT_START, 0);
    dataSegment.resize(STACK_START - DATA_START, 0);

    textTop = TEXT_START;
    dataTop = DATA_START;
    stackTop = STACK_START;
}

void simulator::writeReg(uint32_t regName, int64_t value)
{
    registers[regName].value = value;
}

uint64_t simulator::readReg(uint32_t regName)
{
    return registers[regName].value;
}

void simulator::writeMem(uint32_t addr, uint64_t value, size_t size)
{
    if (addr >= TEXT_START && addr < DATA_START)
    {
        int index = addr - TEXT_START;

        if(index + size > textSegment.size())
        {
            cerr << "Error: Attempt to write out of bounds of text segment!" << endl;
            return;
        }
        
        for (size_t i = 0; i < size; i++)
        {
            textSegment[index + i] = (value >> (i * 8)) & 0xFF;
        }
    }
    else if (addr >= DATA_START && addr < stackTop)
    {
        size_t offset = addr - DATA_START;

        if (offset + size > dataSegment.size())
        {
            cerr << "Error: Attempt to write out of bounds of data segment!" << endl;
            return;
        }

        for (size_t i = 0; i < size; i++)
        {
            dataSegment[offset + i] = (value >> (i * 8)) & 0xFF;
        }
    }
    else if (addr >= stackTop && addr <= STACK_START)
    {
        int index = STACK_START - addr;

        if(index + size > dataSegment.size())
        {
            cerr << "Error: Attempt to write out of bounds of stack segment!" << endl;
            return;
        }

        for (size_t i = 0; i < size; i++)
        {
            dataSegment[index + i] = (value >> (i * 8)) & 0xFF;
        }
    }
    else
    {
        cerr << "Error: Address 0x" << hex << addr << " is out of bounds!" << endl;
    }
}

uint64_t simulator::readMem(uint32_t addr, size_t size)
{
    uint64_t result = 0;

    for (size_t i = 0; i < size; ++i)
    {
        uint64_t byte = 0;
        uint32_t currentAddr = addr + i;

        if (currentAddr >= TEXT_START && currentAddr < DATA_START)
        {
            int index = currentAddr - TEXT_START;
            byte = textSegment[index];
        }
        else if (currentAddr >= DATA_START && currentAddr < STACK_START)
        {
            size_t offset = currentAddr - DATA_START;
            byte = dataSegment[offset];
        }
        else
        {
            cerr << "Error: Address 0x" << hex << currentAddr << " is out of bounds!" << endl;
            return 0;
        }

        result |= (byte << (i * 8));
    }

    return result;
}

void pushStack(uint64_t value, uint32_t size)
{
    uint64_t sp = readReg(2); 

    sp -= size;

    writeMem(sp, value, size);

    writeReg(2, sp);
}

uint64_t popStack(uint32_t size)
{
    uint64_t sp = readReg(2);

    uint64_t value = readMem(sp, size);

    sp += size;

    writeReg(2, sp);

    return value;
}

void simulator::printMem(uint32_t addr, int n)
{
    for (int i = 0; i < n; i++)
    {
        uint32_t currentAddr = addr + i;

        if (currentAddr >= TEXT_START && currentAddr < DATA_START)
        {
            int index = currentAddr - TEXT_START;
            uint8_t byte = textSegment[index];
            cout << "Memory[0x" << hex << currentAddr << "] = 0x" << hex << setfill('0') << setw(2) << (int)byte << endl;
        }
        else if (currentAddr >= DATA_START && currentAddr < stackTop)
        {
            int index = currentAddr - DATA_START;
            uint8_t byte = dataSegment[index];
            cout << "Memory[0x" << hex << currentAddr << "] = 0x" << hex << setfill('0') << setw(2) << (int)byte << endl;
        }
        else if (currentAddr >= stackTop && currentAddr < STACK_START)
        {
            int index = STACK_START - currentAddr;
            cout << "Memory[0x" << hex << currentAddr << "] = 0x" << "Stack memory here" << endl;
        }
        else
        {
            cerr << "Error: Address 0x" << hex << currentAddr << " is out of bounds!" << endl;
            break;
        }
    }
}

void simulator::printRegisters()
{
    cout << "Registers:" << endl;
    for (int i = 0; i < registerCount; i++)
    {
        cout << registers[i].name;
        if (i < 10)
        {
            cout << " ";
        }
        cout << " = 0x" << hex << registers[i].value << endl;
    }
}

void simulator::setData(vector<pair<int64_t, int>> data)
{
    for (const auto &[value, size] : data)
    {
        writeMem(dataTop, value, size);
        dataTop += size;
    }
}

void simulator::setText(vector<string> encodedInstructions)
{
    for (const auto &instruction : encodedInstructions)
    {
        if (textTop + 4 > 0x10000)
        {
            cerr << "Error: Text segment is out of bounds!" << endl;
            break;
        }
        cout << instruction << endl;
        writeMem(textTop, stoul(instruction, nullptr, 16), 4);
        textTop += 4;
    }
}

vector<uint32_t> simulator::fetchInst(uint32_t PC)
{
    int count = textTop / 4;
    vector<uint32_t> instructions(count, 0);
    for (int i = 0; i < count; i++)
    {
        instructions[i] = readMem(PC, 4);
        PC += 4;
    }
    return instructions;
}