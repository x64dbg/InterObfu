#ifndef REGISTER_H
#define REGISTER_H

#include "Utils.h"

struct Register
{
    enum Registers //temporary
    {
        Invalid,
        Eax,
        Ebx,
        Ecx,
        Edx,
        Ebp,
        Esp,
        Esi,
        Edi
    };

    Registers reg;

    explicit Register(Registers reg)
        : reg(reg) { }

    explicit Register()
        : Register(Invalid) { }    

    bool operator==(const Register & other) const
    {
        return reg == other.reg;
    }

    OPNEQ(Register);
};

#endif //REGISTER_H