#include "cpu.h"

uint8_t CPU::Read8(uint32_t addr)
{
    addr &= 0x1FFFFFFF;

    return Bus::Read8(addr);
}

uint16_t CPU::Read16(uint32_t addr)
{
    addr &= 0x1FFFFFFF;

    switch (addr)
    {
    case 0x1f800028: return rfcr;
    }

    return Bus::Read16(addr);
}

uint32_t CPU::Read32(uint32_t addr)
{
    addr &= 0x1FFFFFFF;

    switch (addr)
    {
    case 0x1F000024: return expevt;
    }

    return Bus::Read32(addr);
}

void CPU::Write32(uint32_t addr, uint32_t data)
{
    addr &= 0x1FFFFFFF;

    switch (addr)
    {
    case 0x005f74e4: return;
    case 0x1F000010: mmucr.val = data; return;
    case 0x1F00001C: return;
    case 0x1F800000: return;
    case 0x1F800008: return;
    case 0x1F80000C: return;
    case 0x1F800014: return;
    }

    Bus::Write32(addr, data);
}

void CPU::Write16(uint32_t addr, uint16_t data)
{
    addr &= 0x1FFFFFFF;

    switch (addr)
    {
    case 0x1F800004: return;
    case 0x1F80001C: return;
    case 0x1F800024: rtcor = (data&0x3FF); return;
    case 0x1F800028:
        rfcr = data&0x3FF;
        return;
    case 0x005f7480: return;
    }

    Bus::Write16(addr, data);
}

void CPU::Write8(uint32_t addr, uint8_t data)
{
    addr &= 0x1FFFFFFF;

    switch (addr)
    {
    case 0x1F800004: return;
    case 0x1F900000 ... 0x1F90FFFF: return;
    case 0x1F940000 ... 0x1F94FFFF: return;
    }

    Bus::Write8(addr, data);
}

void CPU::Reset()
{
    pc = 0xA0000000;
    expevt = 0;
    sr.value = 0x70000010;
}

void CPU::Clock()
{
    rfcr++;
    if (rfcr > 0x3FF) rfcr = 0;

    uint16_t instr = Read16(pc);

    if ((instr & 0xF000) == 0xE000)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint32_t i = (int32_t)(int8_t)(instr & 0xff);

        regs[n] = i;

        // printf("mov #%02x, r%d\n", i, n);
        pc += 2;
        return;
    }
    else if ((instr & 0xF0FF) == 0x4028)
    {
        uint8_t n = (instr >> 8) & 0xF;

        uint32_t res = regs[n] <<= 16;
        pc += 2;

        // printf("shll16 r%d\n", n);
        return;
    }
    else if ((instr & 0xF00F) == 0x6009)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        uint16_t tmp = regs[m] >> 16;
        regs[n] = regs[m] << 16;
        regs[n] |= tmp;

        // printf("swap.w r%d, r%d\n", m, n);
        pc += 2;
        return;
    }
    else if ((instr & 0xF0FF) == 0x4018)
    {
        uint8_t n = (instr >> 8) & 0xF;
        
        regs[n] <<= 8;
        pc += 2;

        // printf("shll8 r%d\n", n);

        return;
    }
    else if ((instr & 0xF0FF) == 0x4009)
    {
        uint8_t n = (instr >> 8) & 0xF;
        
        regs[n] >>= 2;
        pc += 2;

        // printf("shll2 r%d\n", n);

        return;
    }
    else if ((instr & 0xF000) == 0x5000)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;
        uint32_t d = instr & 0xF;
        
        // printf("mov.l @(%d, r%d), r%d\n", d, m, n);

        d <<= 2;

        uint32_t addr = regs[m] + d;
        regs[n] = Read32(addr);
        pc += 2;
        return;
    }
    else if ((instr & 0xF00F) == 0x200A)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;
        
        regs[n] ^= regs[m];
        pc += 2;
        // printf("xor r%d, r%d\n", m, n);
        return;
    }
    else if ((instr & 0xF00F) == 0x200E)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;
        
        macl = (uint16_t)regs[n]*(uint16_t)regs[m];
        pc += 2;
        // printf("mulu.w r%d, r%d\n", m, n);
        return;
    }
    else if ((instr & 0xF0FF) == 0x001A)
    {
        uint8_t n = (instr >> 8) & 0xF;

        regs[n] = macl;
        pc += 2;
        // printf("sts macl, r%d\n", n);
        return;
    }
    else if ((instr & 0xF00F) == 0x2008)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        if ((regs[n]&regs[m]) == 0) sr.t = 1;
        else sr.t = 0;
        pc += 2;

        // printf("tst r%d, r%d\n", m, n);
        return;
    }
    else if ((instr & 0xFF00) == 0x8B00)
    {
        uint8_t d = instr & 0xff;

        int disp;
        if ((d&0x80)==0)
            disp = (d & 0xff);
        else
            disp = 0xffffff00 | d;
        
        // printf("bf 0x%08x\n", pc+4+(disp<<1));
        
        if (!sr.t)
            pc = pc+4+(disp<<1);
        else
            pc += 2;
        return;
    }
    else if ((instr & 0xFF00) == 0x8900)
    {
        uint8_t d = instr & 0xff;

        int disp;
        if ((d&0x80)==0)
            disp = (d & 0xff);
        else
            disp = 0xffffff00 | d;
        
        // printf("bf 0x%08x\n", pc+4+(disp<<1));
        
        if (sr.t)
            pc = pc+4+(disp<<1);
        else
            pc += 2;
        return;
    }
    else if ((instr&0xF000) == 0x1000)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;
        uint8_t disp = (instr&0xf);

        // printf("mov.l r%d, @(%d, r%d)\n", m, disp, n);
        disp <<= 2;

        uint32_t addr = regs[n] + disp;

        Write32(addr, regs[m]);
        pc += 2;
        return;
    }
    else if ((instr&0xF000) == 0x7000)
    {
        uint32_t imm = (int32_t)(int8_t)(instr&0xff);
        uint8_t n = (instr >> 8) & 0xF;

        regs[n] += imm;
        pc += 2;

        // printf("add #%02x, r%d\n", imm, n);
        return;
    }
    else if ((instr&0xF0FF) == 0x4021)
    {
        uint8_t n = (instr >> 8) & 0xF;

        sr.t = regs[n] & 1;
        regs[n] = ((int32_t)regs[n]) >> 1;

        pc += 2;
        // printf("shar r%d\n", n);
        return;
    }
    else if ((instr&0xFF00) == 0x8100)
    {
        uint8_t n = (instr >> 4) & 0xF;
        uint8_t disp = (instr&0xf);

        // printf("mov.w r0, @(%d, r%d)\n", disp, n);
        disp <<= 1;

        uint32_t addr = regs[n] + disp;

        Write16(addr, regs[0]);
        pc += 2;
        return;
    }
    else if ((instr&0xFF00) == 0xCB00)
    {
        uint8_t imm = instr & 0xff;
        regs[0] |= imm;

        // printf("or #%02x, r0\n", imm);
        pc += 2;
        return;
    }
    else if ((instr&0xF0FF) == 0x4001)
    {
        uint8_t n = (instr >> 8) & 0xF;

        sr.t = regs[n] & 1;
        regs[n] >>= 1;
        pc += 2;

        // printf("shlr r%d\n", n);
        return;
    }
    else if ((instr&0xF0FF) == 0x4005)
    {
        uint8_t n = (instr >> 8) & 0xF;
        sr.t = regs[n] & 1;
        regs[n] >>= 1;
        regs[n] |= (sr.t<<31);

        pc += 2;
        // printf("rotr r%d\n", n);
        return;
    }
    else if ((instr&0xF00F) == 0x6003)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        regs[n] = regs[m];

        pc += 2;
        // printf("mov r%d, r%d\n", m, n);
        return;
    }
    else if ((instr&0xFF00) == 0xC800)
    {
        uint8_t imm = instr & 0xff;
        if (regs[0]&imm) sr.t = 1;
        else sr.t = 0;
        pc += 2;

        // printf("tst #%02x, r0\n", imm);
        return;
    }
    else if ((instr&0xF0FF) == 0x0083)
    {
        uint8_t n = (instr >> 8) & 0xF;
        pc += 2;
        // printf("pref @r%d\n", n);
        return;
    }
    else if ((instr&0xF0FF) == 0x402B)
    {
        uint8_t n = (instr >> 8) & 0xF;

        // printf("jmp @r%d\n", n);

        uint32_t tmp = regs[n];
        pc += 2;
        Clock();
        pc = tmp;
        return;
    }
    else if (instr == 0x9)
    {
        // printf("nop\n");
        pc += 2;
        return;
    }
    else if ((instr&0xF00F) == 0x2000)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        Write8(regs[n], regs[m]);

        // printf("mov.b r%d, @r%d\n", m, n);
        pc += 2;
        return;
    }
    else if ((instr&0xFF00) == 0x8500)
    {
        uint8_t m = (instr >> 4) & 0xF;
        uint32_t disp = (instr&0xf)<<1;

        // printf("mov.w @(%d, r%d), r0\n", disp>>1, m);

        regs[0] = (int32_t)(int16_t)Read16(regs[m] + disp);
        pc += 2;
        return;
    }
    else if ((instr&0xF00F) == 0x3006)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        if (regs[n] > regs[m]) sr.t = 1;
        else sr.t = 0;

        pc += 2;
        // printf("cmp/hi r%d, r%d\n", m, n);
        return;
    }
    else if ((instr&0xF00F) == 0x6008)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        uint32_t temp0, temp1;
        temp0 = regs[m] & 0xFFFF0000;
        temp1 = (regs[m] & 0xFF) << 8;
        regs[n] = (regs[m] & 0x0000FF00) >> 8;
        regs[n] |= temp1|temp0;
        pc += 2;

        // printf("swap.b r%d, r%d\n", m, n);
        return;
    }
    else if ((instr&0xF00F) == 0x2001)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        Write16(regs[n], regs[m]);

        pc += 2;
        // printf("mov.w r%d, @r%d\n", m, n);
        return;
    }
    else if ((instr&0xFF00) == 0xC700)
    {
        uint32_t d = instr&0xff;

        regs[0] = (pc&0xfffffffc)+4+(d<<2);

        // printf("mova #%08x, r0\n", (pc&0xfffffffc)+4+(d<<2));
        pc += 2;
        return;
    }
    else if ((instr&0xF0FF) == 0x4010)
    {
        uint8_t n = (instr >> 8) & 0xF;

        regs[n] -= 1;
        if (regs[n]) sr.t = 0;
        else sr.t = 1;

        pc += 2;
        // printf("dt r%d\n", n);
        return;
    }
    else if ((instr&0xF00F) == 0x6005)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        regs[n] = (int32_t)(int16_t)Read16(regs[m]);
        regs[m] += 2;

        pc += 2;
        // printf("mov.w @r%d+, r%d\n", m, n);
        return;
    }
    else if ((instr&0xF00F) == 0x2005)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        regs[n] -= 2;
        Write16(regs[n], regs[m]);

        pc += 2;
        // printf("mov.w r%d, @-r%d\n", m, n);
        return;
    }
    else if ((instr&0xF00F) == 0x6002)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        regs[n] = Read32(regs[m]);

        pc += 2;
        // printf("mov.l @r%d, r%d\n", m, n);
        return;
    }
    else if ((instr&0xF000) == 0xD000)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint32_t disp = instr&0xff;

        uint32_t addr = (disp<<2)+(pc&0xfffffffc)+4;

        regs[n] = Read32(addr);
        pc += 2;
        // printf("mov.l @(%d, pc), r%d\n", disp, n);
        return;
    }
    else if ((instr&0xF00F) == 0x2002)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        Write32(regs[n], regs[m]);
        pc += 2;

        // printf("mov.l r%d, @r%d\n", m, n);
        return;
    }
    else if ((instr&0xF00F) == 0x6006)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;

        regs[n] = Read32(regs[m]);
        regs[m] += 4;
        pc += 2;

        // printf("mov.l @r%d+, r%d\n", m, n);
        return;
    }
    else if ((instr&0xF0FF) == 0x40FA)
    {
        uint8_t n = (instr >> 8) & 0xF;

        regs[n] = dbr;
        pc += 2;
        // printf("ldc r%d, dbr\n", n);
        return;
    }
    else if ((instr&0xF0FF) == 0x400b)
    {
        uint8_t n = (instr >> 8) & 0xF;
        pr = pc+4;
        pc += 2;
        uint32_t tmp = regs[n];
        Clock();
        pc = tmp;
        return;
    }
    else if ((instr&0xF0FF) == 0x400e)
    {
        uint8_t n = (instr >> 8) & 0xF;
        sr.value = regs[n];
        pc += 2;
        // printf("ldc r%d, sr\n", n);
        return;
    }
    else if ((instr&0xF0FF) == 0x406a)
    {
        uint8_t n = (instr >> 8) & 0xF;
        fpcsr.value = regs[n];
        pc += 2;
        // printf("ldc r%d, fpcsr\n", n);
        return;
    }
    else if ((instr&0xF00F) == 0xF009)
    {
        uint8_t n = (instr >> 8) & 0xF;
        uint8_t m = (instr >> 4) & 0xF;
        
        fr[fpcsr.fr][n] = (float)Read32(regs[m]);
        regs[m] += 4;

        pc += 2;
        printf("fmov.s @r%d+, r%d\n", m, n);
        return;
    }
    else if (instr == 0xfbfd)
    {
        fpcsr.fr ^= 1;
        pc += 2;
        printf("frchg\n");
        return;
    }

    printf("Unhandled instruction 0x%04x\n", instr);
    exit(1);
}

void CPU::Dump()
{
    for (int i = 0; i < 16; i++)
        printf("r%d\t->\t0x%08x\n", i, regs[i]);
    for (int i = 0; i < 16; i++)
    {
        printf("fr%d_bank0\t->\t%f\n", i, fr[0][i]);
        printf("fr%d_bank1\t->\t%f\n", i, fr[1][i]);
    }
    printf("%s\n", sr.t ? "t" : ".");
    printf("pc\t->\t0x%08x\n", pc);
}
