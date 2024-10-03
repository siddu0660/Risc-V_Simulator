#include <bits/stdc++.h>
#include "assembler.h"
#include "functions.h"

using namespace std;
using namespace assembler;
using namespace simulator;

const array<array<const char *, 3>, 33> Format::registers = {{
    {"x0", "zero", "00000"},
    {"x1", "ra", "00001"},
    {"x2", "sp", "00010"},
    {"x3", "gp", "00011"},
    {"x4", "tp", "00100"},
    {"x5", "t0", "00101"},
    {"x6", "t1", "00110"},
    {"x7", "t2", "00111"},
    {"x8", "s0", "01000"},
    {"x8", "fp", "01000"},
    {"x9", "s1", "01001"},
    {"x10", "a0", "01010"},
    {"x11", "a1", "01011"},
    {"x12", "a2", "01100"},
    {"x13", "a3", "01101"},
    {"x14", "a4", "01110"},
    {"x15", "a5", "01111"},
    {"x16", "a6", "10000"},
    {"x17", "a7", "10001"},
    {"x18", "s2", "10010"},
    {"x19", "s3", "10011"},
    {"x20", "s4", "10100"},
    {"x21", "s5", "10101"},
    {"x22", "s6", "10110"},
    {"x23", "s7", "10111"},
    {"x24", "s8", "11000"},
    {"x25", "s9", "11001"},
    {"x26", "s10", "11010"},
    {"x27", "s11", "11011"},
    {"x28", "t3", "11100"},
    {"x29", "t4", "11101"},
    {"x30", "t5", "11110"},
    {"x31", "t6", "11111"},
}};

const unordered_map<string, tuple<string, string, string>> R::instructionMap = {
    {"add", {"0110011", "000", "0000000"}},
    {"sub", {"0110011", "000", "0100000"}},
    {"xor", {"0110011", "100", "0000000"}},
    {"or", {"0110011", "110", "0000000"}},
    {"and", {"0110011", "111", "0000000"}},
    {"sll", {"0110011", "001", "0000000"}},
    {"srl", {"0110011", "101", "0000000"}},
    {"sra", {"0110011", "101", "0100000"}},
    {"slt", {"0110011", "010", "0000000"}},
    {"sltu", {"0110011", "011", "0000000"}}};

const unordered_map<string, tuple<string, string, string>> I::instructionMap = {
    {"addi", {"0010011", "000", ""}},
    {"xori", {"0010011", "100", ""}},
    {"ori", {"0010011", "110", ""}},
    {"andi", {"0010011", "111", ""}},
    {"slli", {"0010011", "001", "000000"}},
    {"srli", {"0010011", "101", "000000"}},
    {"srai", {"0010011", "101", "100000"}},
    {"slti", {"0010011", "010", ""}},
    {"sltiu", {"0010011", "011", ""}},
    {"lb", {"0000011", "000", ""}},
    {"lh", {"0000011", "001", ""}},
    {"lw", {"0000011", "010", ""}},
    {"ld", {"0000011", "011", ""}},
    {"lbu", {"0000011", "100", ""}},
    {"lhu", {"0000011", "101", ""}},
    {"lwu", {"0000011", "110", ""}},
    {"jalr", {"1100111", "000", ""}},
    {"ecall", {"1110011", "000", ""}},
    {"ebreak", {"1110011", "000", ""}}};

const unordered_map<string, tuple<string, string>> S::instructionMap = {
    {"sb", {"0100011", "000"}},
    {"sh", {"0100011", "001"}},
    {"sw", {"0100011", "010"}},
    {"sd", {"0100011", "011"}}};

const unordered_map<string, tuple<string, string>> B::instructionMap = {
    {"beq", {"1100011", "000"}},
    {"bne", {"1100011", "001"}},
    {"blt", {"1100011", "100"}},
    {"bge", {"1100011", "101"}},
    {"bltu", {"1100011", "110"}},
    {"bgeu", {"1100011", "111"}}};

const unordered_map<string, string> U::instructionMap = {
    {"lui", "0110111"},
    {"auipc", "0010111"}};

const unordered_map<string, string> J::instructionMap = {
    {"jal", "1101111"}};

bool R::isR(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool I::isI(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool S::isS(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool B::isB(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool U::isU(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool J::isJ(const string &inst_name)
{
    return instructionMap.find(inst_name) != instructionMap.end();
}

bool isValid(const string inst_name)
{
    return R::isR(inst_name) || I::isI(inst_name) || S::isS(inst_name) || B::isB(inst_name) || U::isU(inst_name) || J::isJ(inst_name);
}

vector<string> simulator::processInstructions(const vector<string> &instructions, unordered_map<string, int> &labelMap, int PC, const string &errorLog)
{
    ofstream errorFile(errorLog);
    vector<string> encodedInstructions;
    bool hasError = false;

    for (const string &instruction : instructions)
    {
        istringstream iss(instruction);
        string inst_name, rd, rs1, rs2, imm_str;
        int32_t imm = 0;
        string encoded;
        stringstream ss;
        iss >> inst_name;

        if (R::isR(inst_name))
        {
            iss >> rd >> rs1 >> rs2;
            rd = rd.substr(0, rd.length() - 1);
            rs1 = rs1.substr(0, rs1.length() - 1);
            R r_inst(inst_name, rd, rs1, rs2, PC);
            encoded = r_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else if (I::isI(inst_name))
        {
            iss >> rd >> rs1;
            rd = rd.substr(0, rd.length() - 1);
            if (inst_name == "ld" || inst_name == "lw" || inst_name == "lh" || inst_name == "lb" ||
                inst_name == "lbu" || inst_name == "lhu" || inst_name == "lwu" || inst_name =="jalr" )
            {
                size_t open = rs1.find('(');
                size_t close = rs1.find(')');
                if (open != string::npos && close != string::npos)
                {
                    imm_str = rs1.substr(0, open);
                    rs1 = rs1.substr(open + 1, close - open - 1);
                    imm = stoll(imm_str);
                }
            }
            else if (inst_name == "ecall")
            {
                imm = 0;
            }
            else if (inst_name == "ebreak")
            {
                imm = 1;
            }
            else
            {
                rs1 = rs1.substr(0, rs1.length() - 1);
                iss >> imm_str;
                imm = stoll(imm_str);
                if (imm_str[0] == '0' && imm_str[1] == 'x')
                {
                    imm_str = imm_str.substr(2);
                    imm = stoll(imm_str, nullptr, 16);
                }
            }
            I i_inst(inst_name, rd, rs1, imm, PC);
            encoded = i_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else if (S::isS(inst_name))
        {
            iss >> rs2 >> rs1;
            rs2 = rs2.substr(0, rs2.length() - 1);
            size_t open = rs1.find('(');
            size_t close = rs1.find(')');
            if (open != string::npos && close != string::npos)
            {
                imm = stoi(rs1.substr(0, open));
                rs1 = rs1.substr(open + 1, close - open - 1);
            }
            S s_inst(inst_name, rs1, rs2, imm, PC);
            encoded = s_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else if (B::isB(inst_name))
        {
            iss >> rs1 >> rs2 >> imm_str;
            rs1 = rs1.substr(0, rs1.length() - 1);
            rs2 = rs2.substr(0, rs2.length() - 1);
            if (labelMap.find(imm_str) != labelMap.end())
            {
                imm = ((labelMap[imm_str] - 1) * 4) - PC;
            }
            else if(labelMap.find(imm_str) == labelMap.end())
            {
                errorFile << "Line No : " << (PC / 4) + 1 << "" << " Label not found" << endl;
                hasError = true;
                imm = 0;
            }
            else
            {
                imm = stoi(imm_str);
                if (imm_str[0] == '0' && imm_str[1] == 'x')
                {
                    imm_str = imm_str.substr(2);
                    imm = stoll(imm_str, nullptr, 16);
                }
            }
            B b_inst(inst_name, rs1, rs2, imm, PC);
            encoded = b_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else if (U::isU(inst_name))
        {
            iss >> rd >> imm_str;
            rd = rd.substr(0, rd.length() - 1);
            imm = stoi(imm_str);
            if (imm_str[0] == '0' && imm_str[1] == 'x')
            {
                imm_str = imm_str.substr(2);
                imm = stoll(imm_str, nullptr, 16);
            }
            U u_inst(inst_name, rd, imm, PC);
            encoded = u_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else if (J::isJ(inst_name))
        {
            iss >> rd >> imm_str;
            rd = rd.substr(0, rd.length() - 1);

            if (labelMap.find(imm_str) != labelMap.end())
            {
                imm = ((labelMap[imm_str] - 1) * 4) - PC;
            }
            else
            {
                imm = stoi(imm_str);
                if (imm_str[0] == '0' && imm_str[1] == 'x')
                {
                    imm_str = imm_str.substr(2);
                    imm = stoll(imm_str, nullptr, 16);
                }
            }

            J j_inst(inst_name, rd, imm, PC);
            encoded = j_inst.encode();
            encodedInstructions.push_back(encoded);
        }
        else
        {
            errorFile << "Line No : " << (PC)/4 + 1 << " Unsupported instruction: " << inst_name << endl;
            hasError = true;
            break;
        }
        
        PC += 4;
    }

    if (hasError)
    {
        encodedInstructions.clear();
        cout << "Error !! Exiting the program . Check the error.log file for more details" << endl;
        exit(0);
    }
    errorFile.close();

    return encodedInstructions;
}