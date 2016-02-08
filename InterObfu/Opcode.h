#ifndef OPCODE_H
#define OPCODE_H

struct Opcode
{
    enum Mnemonics //temporary
    {
        Invalid,
        Push,
        Pop,
        Mov,
        Sub,
        Add,
        Jmp,
        Call,
        Ret
    };

    Mnemonics mnem;
};

#endif //OPCODE_H