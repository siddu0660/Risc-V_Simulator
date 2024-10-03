#include <bits/stdc++.h>
#include "assembler.h"
#include "functions.h"

using namespace std;
using namespace assembler;
using namespace simulator;

void simulator::executeInstruction(vector<string> instructions, vector<uint32_t> encodedInstructions, uint32_t &PC , bool step, vector<int> &breakpoints)
{
    static bool isHalt = false;

    if(PC/4 >= encodedInstructions.size() && step)
    {
        cout << "Nothing to step" << endl;
    }
    while(PC/4 < encodedInstructions.size())
    {
        if (find(breakpoints.begin(), breakpoints.end(), int(PC / 4)) != breakpoints.end())
        {
            if(!isHalt)
            {
                cout << "Execution Stopped at breakpoint" << endl;
                isHalt = true;
                return;
            }

            isHalt = false;
        }

        cout << "Executed " << instructions[PC / 4] << "; PC = 0x" << hex << setw(8) << setfill('0') << PC << endl;
        uint32_t instruction = encodedInstructions[PC/4];
        uint32_t opcode = instruction & 0x7F;

        switch (opcode)
        {

            case 0b0110011:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                uint32_t rs2 = (instruction >> 20) & 0x1F;
                uint32_t funct7 = instruction >> 25;

                uint64_t rs1Value = readReg(rs1);
                uint64_t rs2Value = readReg(rs2);

                switch (funct3)
                {
                    case 0b000:
                        if (funct7 == 0b0100000)
                        {
                            writeReg(rd, rs1Value - rs2Value);
                        }
                        else
                        {
                            writeReg(rd, rs1Value + rs2Value);
                        }
                        break;

                    case 0b001: 
                        writeReg(rd, rs1Value << rs2Value);
                        break;
                    case 0b010:
                        writeReg(rd, (int64_t)rs1Value < (int64_t)rs2Value ? 1 : 0);
                        break;
                    case 0b011:
                        writeReg(rd, rs1Value < rs2Value ? 1 : 0);
                        break;
                    case 0b100:
                        writeReg(rd, rs1Value ^ rs2Value);
                        break;
                    case 0b101:
                        if (funct7 == 0b0100000)
                        {
                            writeReg(rd, rs1Value >> rs2Value);
                        }
                        else
                        {
                            writeReg(rd, (int64_t)rs1Value >> (int64_t)rs2Value);
                        }
                        break;
                    case 0b110:
                        writeReg(rd, rs1Value | rs2Value);
                        break;
                    case 0b111:
                        writeReg(rd, rs1Value & rs2Value);
                        break;
                }
                break;
            }

            case 0b0010011:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                int32_t imm = (instruction >> 20) & 0xFFF;

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                uint64_t rs1Value = readReg(rs1);
                
                switch (funct3)
                {
                    case 0b000:
                        writeReg(rd, rs1Value + imm);
                        break;
                    case 0b010:
                        writeReg(rd, (int64_t)rs1Value < (int64_t)imm ? 1 : 0);
                        break;
                    case 0b011:
                        writeReg(rd, rs1Value < (uint64_t)imm ? 1 : 0);
                        break;
                    case 0b100:
                        writeReg(rd, rs1Value ^ imm);
                        break;
                    case 0b110:
                        writeReg(rd, rs1Value | imm);
                        break;
                    case 0b111:
                        writeReg(rd, rs1Value & imm);
                        break;
                    case 0b001:
                        writeReg(rd, rs1Value << imm);
                        break;
                    case 0b101:
                        int64_t shiftAmount = imm & 0x1F;
                        bool isArithmeticShift = (imm & 0x400) != 0;

                        if (isArithmeticShift)
                        {
                            writeReg(rd, (int64_t)rs1Value >> shiftAmount);
                        }
                        else
                        {
                            writeReg(rd, (uint64_t)rs1Value >> shiftAmount);
                        }
                        break;
                }
                break;
            }

            case 0b0000011:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                int32_t imm = (instruction >> 20) & 0xFFF;

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                uint64_t rs1Value = readReg(rs1);
                uint32_t addr = rs1Value + imm;
                switch (funct3)
                {
                    case 0b000:
                    {
                        uint64_t byte = readMem(addr, 1);
                        if (byte & 0x80)                  
                        {
                            byte |= 0xFFFFFFFFFFFFFF00; 
                        }
                        writeReg(rd, byte);
                        break;
                    }

                    case 0b001:
                    {
                        uint64_t halfword = readMem(addr, 2); 
                        if (halfword & 0x8000)                
                        {
                            halfword |= 0xFFFFFFFFFFFF0000;
                        }
                        writeReg(rd, halfword);
                        break;
                    }

                    case 0b010:
                    {
                        uint64_t word = readMem(addr, 4);
                        if (word & 0x80000000) 
                        {
                            word |= 0xFFFFFFFF00000000;
                        }
                        writeReg(rd, word);
                        break;
                    }

                    case 0b011:
                    {
                        writeReg(rd, readMem(addr, 8)); 
                        break;
                    }

                    case 0b100:
                    {
                        uint64_t byte = readMem(addr, 1); 
                        byte &= 0xFF;                    
                        writeReg(rd, byte);               
                        break;
                    }

                    case 0b101: 
                    {
                        uint64_t halfword = readMem(addr, 2); 
                        halfword &= 0xFFFF;                   
                        writeReg(rd, halfword);             
                        break;
                    }

                    case 0b110:
                    {
                        uint64_t word = readMem(addr, 4); 
                        word &= 0xFFFFFFFF;            
                        writeReg(rd, word);
                        break;
                    }
                }
                break;
            }

            case 0b0100011:
            {
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                uint32_t rs2 = (instruction >> 20) & 0x1F;
                int32_t imm = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F);

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                uint64_t rs1Value = readReg(rs1);
                uint64_t rs2Value = readReg(rs2);

                uint32_t addr = rs1Value + imm;

                switch (funct3)
                {
                    case 0b000:
                        writeMem(addr, rs2Value & 0xFF, 1);
                        break;
                    case 0b001:
                        writeMem(addr, rs2Value & 0xFFFF, 2);
                        break;
                    case 0b010:
                        writeMem(addr, rs2Value & 0xFFFFFFFF, 4);
                        break;
                    case 0b011:
                        writeMem(addr, rs2Value, 8);
                        break;
                }
                break;
            }

            case 0b1100011:
            {
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                uint32_t rs2 = (instruction >> 20) & 0x1F;
                int32_t imm = ((instruction >> 31) & 0x1) << 12 | ((instruction >> 7) & 0x1) << 11 | ((instruction >> 25) & 0x3F) << 5 | ((instruction >> 8) & 0xF) << 1;

                if (imm & (1 << 12))
                {
                    imm |= 0xFFFFE000;
                }

                uint64_t rs1Value = readReg(rs1);
                uint64_t rs2Value = readReg(rs2);

                switch (funct3)
                {
                    case 0b000:
                        if (rs1Value == rs2Value)
                        {
                            PC += imm - 4;
                        }
                        break;
                    case 0b001:
                        if (rs1Value != rs2Value)
                        {
                            PC += imm - 4;
                        }
                        break;
                    case 0b100:
                        if((int64_t)rs1Value < (int64_t)rs2Value)
                        {
                            PC += imm - 4;
                        }
                        break;
                    case 0b101:
                        if((int64_t)rs1Value >= (int64_t)rs2Value)
                        {
                            PC += imm - 4;
                        }
                        break;
                    case 0b110:
                        if (rs1Value < rs2Value)
                        {
                            PC += imm - 4;
                        }
                    case 0b111:
                        if (rs1Value >= rs2Value)
                        {
                            PC += imm - 4;
                        }
                        break;
                    }
                break;
            }

            case 0b0110111:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                writeReg(rd, instruction & 0xFFFFF000);
                break;
            }

            case 0b0010111:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                writeReg(rd, PC + (instruction & 0xFFFFF000));
                break;
            }

            case 0b1101111:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                int32_t imm = ((instruction >> 31) << 20) | ((instruction >> 12) & 0xFF) << 12 | ((instruction >> 20) & 0x1) << 11 | ((instruction >> 21) & 0x3FF) << 1;

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                writeReg(rd, PC + 4);
                PC += imm - 4;
                break;
            }

            case 0b1100111:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                int32_t imm = (instruction >> 20);
                uint64_t temp = PC + 4;

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                uint64_t rs1Value = readReg(rs1);
                PC = (rs1Value + imm) & ~1UL;
                PC -= 4;
                writeReg(rd, temp);
                break;
            }

            case 0b1110011:
            {
                uint32_t rd = (instruction >> 7) & 0x1F;
                uint32_t funct3 = (instruction >> 12) & 0x7;
                uint32_t rs1 = (instruction >> 15) & 0x1F;
                uint32_t rs2 = (instruction >> 20) & 0x1F;
                int32_t imm = (instruction >> 20);

                if (imm & 0x800)
                {
                    imm |= 0xFFFFF000;
                }

                if (funct3 == 0 && rd == 0 && rs1 == 0 && rs2 == 0)
                {
                    if (imm == 0)
                    {
                        cout << "ECALL !!" << endl;
                        exit(0);
                    }
                    else if (imm == 1)
                    {
                        cout << "EBREAK !!" << endl;
                        return;
                    }
                }
                break;
            }

        }

        PC += 4;
        writeReg(0, 0);

        if(step)
        {
            break;
        }
    }
}