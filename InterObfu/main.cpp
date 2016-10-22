#include "Instruction.h"
#include <cstdio>
#include "State.h"
#include "Pattern.h"
#include "Convert.h"
#include "XEDParse/XEDParse.h"

#ifdef _WIN64
#pragma comment(lib, "XEDParse/XEDParse_x64.lib")
#pragma comment(lib, "capstone_wrapper/capstone/capstone_x64.lib")
#else
#pragma comment(lib, "XEDParse/XEDParse_x86.lib")
#pragma comment(lib, "capstone_wrapper/capstone/capstone_x86.lib")
#endif //_WIN64

static Instruction Assemble(const std::string & text, int mode = 32, uint64_t addr = 0)
{
    Instruction result(X86_INS_NOP);
    XEDPARSE XEDParse;
    XEDParse.x64 = mode == 64;
    XEDParse.cip = addr;
    strncpy_s(XEDParse.instr, text.c_str(), _TRUNCATE);
    if(XEDParseAssemble(&XEDParse) == XEDPARSE_OK)
    {
        csh handle;
        if(cs_open(CS_ARCH_X86, mode == 32 ? CS_MODE_32 : CS_MODE_64, &handle) == CS_ERR_OK)
        {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            const uint8_t* data = XEDParse.dest;
            cs_insn* insn = cs_malloc(handle);
            size_t codeSize = XEDParse.dest_size;
            uint64_t addr64 = addr;
            if(cs_disasm_iter(handle, &data, &codeSize, &addr64, insn))
                result = FromCapstone(insn);
            else
                puts("cs_disasm_iter failed!");
            cs_free(insn, 1);
            cs_close(&handle);
        }
        else
            puts("cs_open failed!");
    }
    else
        printf("XEDParse error: %s\n", XEDParse.error);
    return result;
}

struct WildcardImm : Value
{
    explicit WildcardImm()
        : Value([](const Value & a, const Value & b)
    {
        return true;
    }) { }
};

void basicTest()
{
    //representation of "push 0x300"
    auto push300 = Instruction(X86_INS_PUSH, Operand(Value(0x300)));

    //representation of "push imm"
    auto pushImm = Instruction(X86_INS_PUSH, Operand(WildcardImm()));

    Instruction push300_;
    push300_.opcode.mnem = X86_INS_PUSH;
    push300_.opCount = 1;
    push300_.operands[0].type = Operand::Imm;
    push300_.operands[0].imm.val = 0x300;

    printf("equal: %d %d %d\n", push300 == push300_, push300 == pushImm, pushImm == push300);

    //representation of "mov eax, ebx"
    Instruction movReg;
    movReg.opcode.mnem = X86_INS_MOV;
    movReg.opCount = 2;
    movReg.operands[0].type = Operand::Reg;
    movReg.operands[0].reg.reg = Register::EAX;
    movReg.operands[1].type = Operand::Reg;
    movReg.operands[1].reg.reg = Register::EBX;

    //representation of "mov [ebx + 0x401000], 0x3"
    Instruction movMem;
    movMem.opcode.mnem = X86_INS_MOV;
    movMem.opCount = 2;
    movMem.operands[0].type = Operand::Mem;
    movMem.operands[0].mem.base.reg = Register::EBX;
    movMem.operands[0].mem.disp.val = 0x401000;
    movMem.operands[1].type = Operand::Imm;
    movMem.operands[1].imm.val = 0x3;

    Pattern pattern1;
    //push reg2
    auto pushReg = Instruction(X86_INS_PUSH, Operand(Register(pattern1.state.MakeRegisterN(2))));
    pattern1.Add(pushReg);
    //mov [memory1], 0x3
    auto movMemImm1 = Instruction(X86_INS_MOV, Operand(Memory(pattern1.state.MakeMemoryN(1))), Operand(Value(3)));
    pattern1.Add(movMemImm1);

    Pattern pattern2;
    //mov [base1 + index2 * 1 + 0], imm1
    auto base1 = Register(pattern2.state.MakeRegisterN(1));
    auto index2 = Register(pattern2.state.MakeRegisterN(2));
    auto movMemImm2 = Instruction(X86_INS_MOV, Operand(Memory(base1, index2, Value(1), Value(0))), Operand(Value(pattern2.state.MakeValueN(1))));
    pattern2.Add(movMemImm2);
    //xor reg1, reg2
    auto xorRegReg = Instruction(X86_INS_XOR, Operand(Register(pattern2.state.MakeRegisterN(1))), Operand(Register(pattern2.state.MakeRegisterN(2))));
    pattern2.Add(xorRegReg);
}

void checkEqual(const Instruction & a, const Instruction & b)
{
    if(a == b)
        puts("a == b");
    else
        puts("a != b");
}

void matchTest2()
{
    std::vector<Instruction> ins1;
    ins1.push_back(Assemble("xor eax, 0"));
    ins1.push_back(Assemble("nop"));
    ins1.push_back(Assemble("push ecx"));
    ins1.push_back(Assemble("push eax"));
    ins1.push_back(Assemble("pop ecx"));
    ins1.push_back(Assemble("nop"));

    Pattern pat1;
    pat1.Add(Instruction(X86_INS_PUSH, Operand(Register(pat1.state.MakeRegisterN(1))))); //push reg1
    pat1.Add(Instruction(X86_INS_POP, Operand(Register(pat1.state.MakeRegisterN(2))))); //pop reg2
    printf("match1: %lld\n", pat1.Match(ins1));

    std::vector<Instruction> ins2;
    ins2.push_back(Assemble("xor eax, 0"));
    ins2.push_back(Assemble("push ecx"));
    ins2.push_back(Assemble("push edx"));
    ins2.push_back(Assemble("pop edx"));
    ins2.push_back(Assemble("nop"));

    Pattern pat2;
    pat2.Add(Instruction(X86_INS_PUSH, Operand(Register(pat2.state.MakeRegisterN(1))))); //push reg1
    pat2.Add(Instruction(X86_INS_POP, Operand(Register(pat2.state.MakeRegisterN(1))))); //pop reg1
    printf("match2: %lld\n", pat2.Match(ins2));

    std::vector<Instruction> ins3;
    ins3.push_back(Assemble("mov dword ptr ds:[eax+ebx*2], ecx"));
    ins3.push_back(Assemble("mov ecx, dword ptr ds:[eax+ebx*2]"));

    Pattern pat3;
    pat3.Add(Instruction(X86_INS_MOV, Operand(Memory(pat3.state.MakeMemoryN(1))), Operand(pat3.state.MakeOperandN(1)))); //mov memory1, op1
    pat3.Add(Instruction(X86_INS_MOV, Operand(pat3.state.MakeOperandN(1)), Operand(Memory(pat3.state.MakeMemoryN(1))))); //mov op1, memory1
    printf("match3: %lld\n", pat3.Match(ins3));
}

void matchTest()
{
    auto movEaxEbx = Assemble("mov eax, ebx");
    auto movRcxRcx = Assemble("mov rcx, rcx", 64);

    State s;
    //mov reg1, reg2
    auto movReg1Reg2 = Instruction(X86_INS_MOV, Operand(Register(s.MakeRegisterN(1))), Operand(Register(s.MakeRegisterN(2))));
    checkEqual(movEaxEbx, movReg1Reg2);
    s.Clear();

    //mov reg1, reg1
    auto movReg1Reg1 = Instruction(X86_INS_MOV, Operand(Register(s.MakeRegisterN(1))), Operand(Register(s.MakeRegisterN(1))));
    checkEqual(movEaxEbx, movReg1Reg1);
    s.Clear();

    checkEqual(movRcxRcx, movReg1Reg1);
}

int main()
{
    matchTest2();

    system("pause");

    return 0;
}