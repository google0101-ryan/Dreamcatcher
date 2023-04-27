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
    uint32_t dbr = 0;
    uint32_t pr = 0;

    union
    {
        uint32_t value;
        struct
        {
            uint32_t t : 1;
            uint32_t s : 1;
            uint32_t : 2;
            uint32_t imask : 4;
            uint32_t q : 1;
            uint32_t m : 1;
            uint32_t : 5;
            uint32_t fd : 1;
            uint32_t : 12;
            uint32_t bl : 1;
            uint32_t rb : 1;
            uint32_t md : 1;
            uint32_t : 1;
        };
    } sr;

    union
    {
        uint32_t value;
        struct
        {
            uint32_t rm : 2;
            uint32_t fi : 1;
            uint32_t fu : 1;
            uint32_t fo : 1;
            uint32_t fdbz : 1;
            uint32_t fio : 1;
            uint32_t ei : 1;
            uint32_t eu : 1;
            uint32_t eo : 1;
            uint32_t edbz : 1;
            uint32_t eio : 1;
            uint32_t ci : 1;
            uint32_t cu : 1;
            uint32_t co : 1;
            uint32_t cdbz : 1;
            uint32_t cio : 1;
            uint32_t cfe : 1;
            uint32_t dn : 1;
            uint32_t pr : 1;
            uint32_t sz : 1;
            uint32_t fr : 1;
            uint32_t : 10;
        };
    } fpcsr;

    float fr[2][15];

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