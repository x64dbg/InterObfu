#include "Convert.h"

std::unordered_map<std::string, Register::Registers> Converter::mstr2reg;
std::unordered_map<Register::Registers, std::string> Converter::mreg2str;
std::unordered_map<std::string, Opcode::Mnemonics> Converter::mstr2ins;
std::unordered_map<Opcode::Mnemonics, std::string> Converter::mins2str;

void Converter::init()
{
    csh handle;
    if(cs_open(CS_ARCH_X86, CS_MODE_32, &handle) == CS_ERR_OK)
    {
        for(auto i = 0; i < int(X86_REG_ENDING); i++)
        {
            if(i == X86_REG_INVALID)
                continue;
            auto regname = cs_reg_name(handle, i);
            if(!regname)
                __debugbreak(); //invalid register
            mstr2reg[regname] = Register::Registers(i);
            mreg2str[Register::Registers(i)] = regname;
        }

        for(auto i = 0; i < int(X86_INS_ENDING); i++)
        {
            if(i == X86_INS_INVALID)
                continue;
            auto insname = cs_insn_name(handle, i);
            if(!insname)
                __debugbreak(); //invalid instruction
            mstr2ins[insname] = Opcode::Mnemonics(i);
            mins2str[Opcode::Mnemonics(i)] = insname;
        }
    }
    cs_close(&handle);
}

Register::Registers Converter::str2reg(const std::string & str)
{
    return k2v(mstr2reg, str);
}

std::string Converter::reg2str(Register::Registers reg)
{
    return k2v(mreg2str, reg);
}

Opcode::Mnemonics Converter::str2ins(const std::string & str)
{
    return k2v(mstr2ins, str);
}

std::string Converter::ins2str(Opcode::Mnemonics ins)
{
    return k2v(mins2str, ins);
}