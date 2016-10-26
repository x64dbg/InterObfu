#pragma once

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