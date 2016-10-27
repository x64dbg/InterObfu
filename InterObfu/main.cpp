#include "Instruction.h"
#include <cstdio>
#include "State.h"
#include "Pattern.h"
#include "Assemble.h"
#include "Peephole.h"

struct WildcardImm : Value
{
    explicit WildcardImm()
        : Value([](const Value & a, const Value & b, State & state)
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

    State s;
    printf("equal: %d %d %d\n", push300.Equals(push300_, s), push300.Equals(pushImm, s), pushImm.Equals(push300, s));

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
    auto pushReg = Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(2))));
    pattern1.Add(pushReg);
    //mov [memory1], 0x3
    auto movMemImm1 = Instruction(X86_INS_MOV, Operand(Memory(State::MakeMemoryN(1))), Operand(Value(3)));
    pattern1.Add(movMemImm1);

    Pattern pattern2;
    //mov [base1 + index2 * 1 + 0], imm1
    auto base1 = Register(State::MakeRegisterN(1));
    auto index2 = Register(State::MakeRegisterN(2));
    auto movMemImm2 = Instruction(X86_INS_MOV, Operand(Memory(base1, index2, Value(1), Value(0))), Operand(Value(State::MakeValueN(1))));
    pattern2.Add(movMemImm2);
    //xor reg1, reg2
    auto xorRegReg = Instruction(X86_INS_XOR, Operand(Register(State::MakeRegisterN(1))), Operand(Register(State::MakeRegisterN(2))));
    pattern2.Add(xorRegReg);
}

void checkEqual(const Instruction & a, const Instruction & b)
{
    State s;
    if(a.Equals(b, s))
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
    pat1.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1))))); //push reg1
    pat1.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2))))); //pop reg2
    printf("match1: %lld\n", pat1.Search(ins1));

    std::vector<Instruction> ins2;
    ins2.push_back(Assemble("xor eax, 0"));
    ins2.push_back(Assemble("push ecx"));
    ins2.push_back(Assemble("push edx"));
    ins2.push_back(Assemble("pop edx"));
    ins2.push_back(Assemble("nop"));

    Pattern pat2;
    pat2.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1))))); //push reg1
    pat2.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1))))); //pop reg1
    printf("match2: %lld\n", pat2.Search(ins2));

    std::vector<Instruction> ins3;
    ins3.push_back(Assemble("mov dword ptr ds:[eax+ebx*2], ecx"));
    ins3.push_back(Assemble("mov ecx, dword ptr ds:[eax+ebx*2]"));

    Pattern pat3;
    pat3.Add(Instruction(X86_INS_MOV, Operand(Memory(State::MakeMemoryN(1))), Operand(State::MakeOperandN(1)))); //mov memory1, op1
    pat3.Add(Instruction(X86_INS_MOV, Operand(State::MakeOperandN(1)), Operand(Memory(State::MakeMemoryN(1))))); //mov op1, memory1
    printf("match3: %lld\n", pat3.Search(ins3));
}

void matchTest()
{
    auto movEaxEbx = Assemble("mov eax, ebx");
    auto movRcxRcx = Assemble("mov rcx, rcx", 64);

    State s;
    //mov reg1, reg2
    auto movReg1Reg2 = Instruction(X86_INS_MOV, Operand(Register(State::MakeRegisterN(1))), Operand(Register(State::MakeRegisterN(2))));
    checkEqual(movEaxEbx, movReg1Reg2);
    s.Clear();

    //mov reg1, reg1
    auto movReg1Reg1 = Instruction(X86_INS_MOV, Operand(Register(State::MakeRegisterN(1))), Operand(Register(State::MakeRegisterN(1))));
    checkEqual(movEaxEbx, movReg1Reg1);
    s.Clear();

    checkEqual(movRcxRcx, movReg1Reg1);
}

#include "aho-corasick.h"

void testAhoCorasick()
{
    /*
    var dict = new[] {"a", "ab", "bab", "bc", "bca", "c", "caa"};
    var haystack = "abccab"
    a = X86_INS_MOV
    b = X86_INS_PUSH
    c = X86_INS_XOR
    */

    std::basic_string<char> input("abccab");
    puts("input:");
    puts(input.c_str());
    puts("");

    aho_corasick::basic_trie<char> trie;
    //trie.remove_overlaps(); //this has undesired behavior because it's not fine-grained enough
    auto dict = std::vector<std::string> {"a", "ab", "bab", "bc", "bca", "c", "caa"};
    puts("strings:");
    unsigned index = 0;
    for(auto & d : dict)
    {
        printf("%u:%s\n", index++, d.c_str());
        trie.insert(d);
    }
    puts("");

    auto results = trie.parse_text(input);
    puts("results (unfiltered):");
    for(auto & r : results)
        printf("%u:%d-%d \"%s\"\n", r.get_index(), int(r.get_start()), int(r.get_end()), r.get_keyword().c_str());
    puts("");

    //This creates a map from (match start) -> (result index).
    //Before considering a result it is checked with a predicate.
    //On conflicting (match start) we use a predicate.

    auto validPredicate = [](const aho_corasick::emit<char> & r) //is r a valid result?
    {
        return true; //TODO: implement by trying a full pattern match with wildcards
    };

    auto betterPredicate = [](const aho_corasick::emit<char> & a, const aho_corasick::emit<char> & b) //is a better than b?
    {
        if(a.size() > b.size()) //longer patterns are always better
            return true;
        return a.get_start() < b.get_start(); //patterns that start earlier are better
    };

    std::unordered_map<size_t, size_t> best;
    best.reserve(results.size());
    for(size_t i = 0; i < results.size(); i++)
    {
        const auto & r = results.at(i);
        if(!validPredicate(r)) //skip invalid results
            continue;
        auto found = best.find(r.get_start());
        if(found == best.end()) //add the current result if not found
            best[r.get_start()] = i;
        else if(betterPredicate(r, results[found->second])) //replace if the current result better
            found->second = i;
    }

    //TODO: overlapping results should be filtered with the betterPredicate

    puts("results (filtered):");
    for(const auto & it : best)
    {
        const auto & r = results[it.second];
        printf("%u:%d-%d \"%s\"\n", r.get_index(), int(r.get_start()), int(r.get_end()), r.get_keyword().c_str());
    }
    puts("");
}

void testConverter()
{
    Converter::init();

    if(Converter::reg2str(Converter::str2reg("eax")) == "eax")
        puts("register 1 ok!");
    else
        puts("register 1 failed...");
    if(Converter::str2reg("asd") == Register::INVALID)
        puts("register 2 ok!");
    else
        puts("register 2 failed...");
    if(Converter::reg2str(Register::Registers(-1)).empty())
        puts("register 3 ok!");
    else
        puts("register 3 failed...");

    if(Converter::ins2str(Converter::str2ins("push")) == "push")
        puts("instruction 1 ok!");
    else
        puts("instruction 1 failed...");
    if(Converter::str2ins("asd") == X86_INS_INVALID)
        puts("instruction 2 ok!");
    else
        puts("instruction 2 failed...");
    if(Converter::ins2str(Opcode::Mnemonics(-1)).empty())
        puts("instruction 3 ok!");
    else
        puts("instruction 3 failed...");
}

void testPeephole()
{
    std::vector<Instruction> ins1;
    ins1.push_back(Assemble("shl eax, 0"));
    ins1.push_back(Assemble("push edx"));
    ins1.push_back(Assemble("push ecx"));
    ins1.push_back(Assemble("pop ecx"));
    ins1.push_back(Assemble("pop edx"));
    ins1.push_back(Assemble("push eax"));
    ins1.push_back(Assemble("pop ecx"));
    ins1.push_back(Assemble("mov eax, 0x1234"));

    std::vector<Pattern> patterns;

    {
        Pattern pat1;
        pat1.Add(Instruction(X86_INS_SHL, Operand(Register(State::MakeRegisterN(1))), Operand(Value(0))));
        patterns.push_back(pat1);
    }

    {
        Pattern pat2;
        pat2.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
        pat2.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
        patterns.push_back(pat2);
    }

    {
        Pattern pat3;
        pat3.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
        pat3.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));
        pat3.repls.push_back([](const State & state)
        {
            return Instruction(X86_INS_MOV, Operand(state.registers[2]()), Operand(state.registers[1]()));
        });
        patterns.push_back(pat3);
    }

    Converter::init();
    Peephole peephole(patterns);
    const auto MaxPasses = 1000;
    auto printInstrs = [](const std::vector<Instruction> & instrs)
    {
        puts("========");
        for(auto & instr : instrs)
            puts(instr.ToString().c_str());
        puts("========");
    };
    puts("input:");
    printInstrs(ins1);
    for(auto i = 0; i < MaxPasses; i++)
    {
        std::vector<Instruction> optimized;
        if(!peephole.Optimize(ins1, optimized))
            break;
        ins1 = optimized;
        printf("pass %d:\n", i + 1);
        printInstrs(ins1);
    }
    puts("output:");
    printInstrs(ins1);
}

int main()
{
    testPeephole();
    //testAhoCorasick();
    //basicTest();
    //matchTest();
    //matchTest2();
    //testConverter();

    system("pause");

    return 0;
}