#ifndef REGISTER_H
#define REGISTER_H

struct Register
{
    enum Registers
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
};

#endif //REGISTER_H