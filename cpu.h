#pragma once

#include <cstdint>
#include <bus.h>

class CPU
{
private:
    uint32_t pc;
    uint32_t regs[16];
    uint32_t expevt;

    uint32_t macl, mach;
    uint16_t rtcor;
    uint16_t rfcr;

    bool t = false;

    union
    {
        uint32_t val;
        struct
        {
            uint32_t at : 1;
            uint32_t : 1;
            uint32_t ti : 1;
            uint32_t : 5;
            uint32_t sv : 1;
            uint32_t sqmd : 1;
            uint32_t urc : 6;
            uint32_t : 2;
            uint32_t urb : 6;
            uint32_t : 2;
            uint32_t lrui : 6;
        };
    } mmucr;

    uint8_t Read8(uint32_t addr);
    uint16_t Read16(uint32_t addr);
    uint32_t Read32(uint32_t addr);

    void Write32(uint32_t addr, uint32_t data);
    void Write16(uint32_t addr, uint16_t data);
    void Write8(uint32_t addr, uint8_t data);
public:
    void Reset();

    void Clock();

    void Dump();
};