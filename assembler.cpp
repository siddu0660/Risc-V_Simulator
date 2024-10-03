#include <bits/stdc++.h>
#include "assembler.h"
using namespace std;
using namespace assembler;

ofstream errorFile("error.log");

uint32_t Format::fetchRegister(const string &reg, int PC)
{
    for (const auto &x : registers)
    {
        if (reg == x[0] || reg == x[1])
        {
            return stoll(x[2], nullptr, 2);
        }
    }
    errorFile << "Line No : " << (PC / 4) + 1 << "" << " Invalid register : " << reg << endl;
    cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl;
    exit(0);
    return 0;
}

void R::setValues(const string &inst_name, const string &rd_str, const string &rs1_str, const string &rs2_str , int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(get<0>(it->second), nullptr, 2);
        funct3 = stoll(get<1>(it->second), nullptr, 2);
        funct7 = stoll(get<2>(it->second), nullptr, 2);
    }
    else
    {
        errorFile << "Line No : " << (PC / 4) + 1 << "" << " Invalid instruction : " << inst_name << endl;
        cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
        exit(0);
    }

    rd = fetchRegister(rd_str, PC);
    rs1 = fetchRegister(rs1_str, PC);
    rs2 = fetchRegister(rs2_str, PC);
}

string R::encode() const
{
    uint32_t encoded = (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << encoded;
    return ss.str();
}

void I::setValues(const string &inst_name, const string &rd_str, const string &rs1_str, const int32_t &imm, int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(get<0>(it->second), nullptr, 2);
        funct3 = stoll(get<1>(it->second), nullptr, 2);
        string funct6_str = get<2>(it->second);
        if (funct6_str.length() == 6)
        {
            bitset<6>(funct6_str).to_ulong();
            funct6 = stoll(funct6_str, nullptr, 2);
            isShift = true;
        }
    }
    if(inst_name != "ecall" && inst_name != "ebreak")
    {
        rd = fetchRegister(rd_str, PC);
        rs1 = fetchRegister(rs1_str, PC);
    }
    else
    {
        rd = 0;
        rs1 = 0;
    }
    if (imm > 2047 || imm < -2048)
    {
        errorFile << "Line No : " << (PC / 4) + 1 << "" << " Immediate value out of range! Must be between -2048 and 2047." << endl;
        cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
        exit(0);
    }

    if (!get<2>(it->second).empty())
    {
        if (imm > 63 || imm < 0)
        {
            errorFile << "Line No : " << (PC / 4) + 1 << "" << " Immediate value out of range for shift instruction! Must be between 0 and 63." << endl;
            cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
            exit(0);
        }
    }
    this->imm = imm;
}

string I::encode() const
{
    bitset<12> imm_12bit(imm);
    uint32_t imm_11_0 = imm_12bit.to_ulong();
    uint32_t imm_5_0 = imm_12bit.to_ulong() & 0x3F;
    uint32_t encoded = 0;

    if(isShift)
    {
        encoded = (funct6 << 25) | (imm_5_0 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
    }
    else
    {
        encoded = (imm_11_0 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
    }

    stringstream ss;
    ss << setfill('0') << setw(8) << hex  << encoded;
    return ss.str();
}

void S::setValues(const string &inst_name, const string &rs1_str, const string &rs2_str, const int32_t &imm, int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(get<0>(it->second), nullptr, 2);
        funct3 = stoll(get<1>(it->second), nullptr, 2);
    }

    rs1 = fetchRegister(rs1_str, PC);
    rs2 = fetchRegister(rs2_str, PC);
    if (imm > 2047 || imm < -2048)
    {
        errorFile << "Line No : " << (PC / 4) + 1 << "" << " Immediate value out of range! Must be between -2048 and 2047." << endl;
        cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
        exit(0);
    }
    this->imm = imm;
}

string S::encode() const
{
    std::bitset<12> imm_12bit(imm);

    if (imm_12bit[11])
    {
        std::bitset<32> imm_32bit;
        imm_32bit |= imm_12bit.to_ulong();
        imm_32bit |= std::bitset<32>(0xFFFFF000);
        imm_12bit = std::bitset<12>(imm_32bit.to_ulong());
    }

    uint32_t imm_11_5 = (imm_12bit.to_ulong() >> 5) & 0x7F;
    uint32_t imm_4_0 = imm_12bit.to_ulong() & 0x1F;

    uint32_t encoded = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm_4_0 << 7) | opcode;

    stringstream ss;
    ss << setfill('0') << setw(8) << hex << encoded;
    return ss.str();
}

void B::setValues(const string &inst_name, const string &rs1_str, const string &rs2_str, const int32_t &imm, int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(get<0>(it->second), nullptr, 2);
        funct3 = stoll(get<1>(it->second), nullptr, 2);
    }

    rs1 = fetchRegister(rs1_str, PC);
    rs2 = fetchRegister(rs2_str, PC);
    if (imm > 4094 || imm < -4096)
    {
        errorFile << "Line No : " << (PC / 4) + 1 << "" << " Immediate value out of range! Must be between -4096 and 4094." << endl;
        cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
        exit(0);
    }
    this->imm = imm;
}

string B::encode() const
{
    uint32_t imm_12 = (imm >> 12) & 1;
    uint32_t imm_10_5 = (imm >> 5) & 0x3F;
    uint32_t imm_4_1 = (imm >> 1) & 0xF;
    uint32_t imm_11 = (imm >> 11) & 1;
    uint32_t encoded;

    if (imm < 0)
    {
    encoded = (imm_12 << 31) |
                        (imm_11 << 7) |
                        (imm_10_5 << 25) |
                        (rs2 << 20) |
                        (rs1 << 15) |
                        (funct3 << 12) |
                        (imm_4_1 << 8) |
                        (opcode);
    }
    else
    {
    encoded = (imm_12 << 30) |
                    (imm_11 << 7) |
                    (imm_10_5 << 25) |
                    (rs2 << 20) |
                    (rs1 << 15) |
                    (funct3 << 12) |
                    (imm_4_1 << 8) |
                    (opcode);
    }

    stringstream ss;
    ss << setfill('0') << setw(8) << hex << encoded;
    return ss.str();
}

void U::setValues(const string &inst_name, const string &rd_str, const int32_t &imm, int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(it->second, nullptr, 2);
    }
    rd = fetchRegister(rd_str, PC);
    this->imm = imm;
}

string U::encode() const
{
    bitset<32> immBits(imm);
    uint32_t encoded = (immBits.to_ulong() << 12) | (rd << 7) | opcode;
    stringstream ss;
    ss << setfill('0') << setw(8) << hex << encoded;
    return ss.str();
}

void J::setValues(const string &inst_name, const string &rd_str, const int32_t &imm, int PC)
{
    auto it = instructionMap.find(inst_name);
    if (it != instructionMap.end())
    {
        opcode = stoll(it->second, nullptr, 2);
    }
    rd = fetchRegister(rd_str, PC);
    if(imm > 1048575 || imm < -1048576)
    {
        errorFile << "Line No : " << (PC / 4) + 1 << "" << " Immediate value out of range! Must be between -1048576 and 1048575." << endl;
        cerr << "Error !! Exiting the program . Check the error.log file for more details" << endl; 
        exit(0);
    }
    this->imm = imm;
}

string J::encode() const
{
    bitset<21> immBits(imm);

    uint32_t imm_20 = (imm >> 19) & 0x1;     
    uint32_t imm_19_12 = (imm >> 12) & 0xFF;
    uint32_t imm_11 = (imm >> 11) & 0x1;     
    uint32_t imm_10_1 = (imm >> 1) & 0x3FF;

    uint32_t encoded;

    if (imm < 0)
    {
        encoded = (imm_20 << 31) | (imm_10_1 << 21) | (imm_11 << 20) | (imm_19_12 << 12) | rd << 7 | opcode;
    }
    else
    {
        encoded = (imm_20 << 31) | (imm_10_1 << 21) | (imm_11 << 20) | (imm_19_12 << 12) | rd << 7 | opcode;
    }

    stringstream ss;
    ss << setfill('0') << setw(8) << hex << encoded;
    return ss.str();
}