#include "Instruction.h"
#include "Convert.h"

std::string Value::ToString() const
{
    char value[31] = "";
    sprintf_s(value, "0x%llX", val);
    return value;
}

std::string Segment::ToString() const
{
    switch(seg)
    {
    case CS:
        return "cs";
    case SS:
        return "ss";
    case DS:
        return "ds";
    case ES:
        return "es";
    case FS:
        return "fs";
    case GS:
        return "gs";
    default:
        return "?";
    }
}

std::string Register::ToString() const
{
    auto str = Converter::reg2str(reg);
    return str.empty() ? "?" : str;
}

std::string Opcode::ToString() const
{
    auto str = Converter::ins2str(mnem);
    return str.empty() ? "?" : str;
}

std::string Memory::ToString() const
{
    std::string str;
    str.push_back('[');
    str.append(base.ToString());
    str.push_back('+');
    str.append(index.ToString());
    str.push_back('*');
    str.append(scale.ToString());
    str.push_back('+');
    str.append(disp.ToString());
    str.push_back(']');
    return str;
}

std::string Operand::ToString() const
{
    switch(type)
    {
    case Reg:
        return reg.ToString();
    case Imm:
        return imm.ToString();
    case Mem:
        return mem.ToString();
    default:
        return "?";
    }
}

std::string Instruction::ToString() const
{
    std::string str;
    str.append(opcode.ToString());

    if(opCount)
        str.push_back(' ');
    for(auto i = 0; i < opCount; i++)
    {
        if(i)
        {
            str.push_back(',');
            str.push_back(' ');
        }
        str.append(operands[i].ToString());
    }
    return str;
}





