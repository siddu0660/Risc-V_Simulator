#include <bits/stdc++.h>

using namespace std;

namespace assembler
{
    class Format
    {
        protected:
            static const array<array<const char *, 3>, 33> registers;
            static uint32_t fetchRegister(const string &reg, int PC);
    };

    class R : public Format
    {
        public:
            R(const string &inst_name, const string &rd, const string &rs1, const string &rs2, int PC)
            {
                setValues(inst_name, rd, rs1, rs2, PC);
            };
            string encode() const;
            static bool isR(const string &inst_name);

        private:
            static const unordered_map<string, tuple<string, string, string>> instructionMap;
            void setValues(const string &inst_name, const string &rd, const string &rs1, const string &rs2, int PC);

            uint32_t opcode : 7;
            uint32_t rd : 5;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            uint32_t funct7 : 7;
    };

    class I : public Format
    {
        public:
            I(const string &inst_name, const string &rd, const string &rs1, const int32_t &imm, int PC)
            {
                setValues(inst_name, rd, rs1, imm, PC);
            };
            string encode() const;
            static bool isI(const string &inst_name);

        private:
            static const unordered_map<string, tuple<string, string, string>> instructionMap;
            void setValues(const string &inst_name, const string &rd, const string &rs1, const int32_t &imm , int PC);

            bool isShift = false;
            uint32_t opcode : 7;
            uint32_t rd : 5;
            uint32_t funct3 : 3;
            uint32_t funct6 : 6;
            uint32_t rs1 : 5;
            uint32_t imm : 12;
    };

    class S : public Format
    {
        public:
            S(const string &inst_name, const string &rs1, const string &rs2, const uint32_t &imm , int PC)
            {
                setValues(inst_name, rs1, rs2, imm, PC);
            };
            string encode() const;
            static bool isS(const string &inst_name);

        private:
            static const unordered_map<string, tuple<string, string>> instructionMap;
            void setValues(const string &inst_name, const string &rs1, const string &rs2, const int32_t &imm, int PC);

            uint32_t opcode : 7;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            int32_t imm;
    };

    class B : public Format
    {
        public:
            B(const string &inst_name, const string &rs1, const string &rs2, const uint32_t &imm, int PC)
            {
                setValues(inst_name, rs1, rs2, imm , PC);
            };
            string encode() const;
            static bool isB(const string &inst_name);

        private:
            static const unordered_map<string, tuple<string, string>> instructionMap;
            void setValues(const string &inst_name, const string &rs1, const string &rs2, const int32_t &imm , int PC);

            uint32_t opcode : 7;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            int32_t imm : 12;
    };

    class U : public Format
    {
        public:
            U(const string &inst_name, const string &rd, const int32_t &imm , int PC)
            {
                setValues(inst_name, rd, imm , PC);
            };
            string encode() const;
            static bool isU(const string &inst_name);

        private:
            static const unordered_map<string, string> instructionMap;
            void setValues(const string &inst_name, const string &rd, const int32_t &imm , int PC);

            uint32_t opcode : 7;
            uint32_t rd : 5;
            int32_t imm : 20;
    };

    class J : public Format
    {
        public:
            J(const string &inst_name, const string &rd, const int32_t &imm , int PC)
            {
                setValues(inst_name, rd, imm , PC);
            };
            string encode() const;
            static bool isJ(const string &inst_name);

        private:
            static const unordered_map<string, string> instructionMap;
            void setValues(const string &inst_name,const string &rd, const int32_t &imm , int PC);

            uint32_t opcode : 7;
            uint32_t rd : 5;
            int32_t imm : 20;
    };
}