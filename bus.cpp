#include "bus.h"

#include <fstream>

uint8_t bootRom[0x200000];
uint8_t flashRom[0x20000];

void Bus::LoadRoms(std::string bootrom, std::string flashrom)
{
    std::ifstream br(bootrom, std::ios::binary | std::ios::ate);
    size_t size = br.tellg();

    if (size != 0x200000)
    {
        printf("ERROR: Invalid BIOS, expected 0x200000, got 0x%lx\n", size);
        exit(1);
    }

    br.seekg(0, std::ios::beg);
    br.read((char*)bootRom, 0x200000);
    br.close();

    std::ifstream fr(flashrom, std::ios::binary | std::ios::ate);
    size = fr.tellg();

    if (size != 0x20000)
    {
        printf("ERROR: Invalid BIOS, expected 0x40000, got 0x%lx\n", size);
        exit(1);
    }

    fr.seekg(0, std::ios::beg);
    fr.read((char*)flashRom, 0x20000);
    fr.close();
}

uint8_t Bus::Read8(uint32_t addr)
{
    printf("Read32 from unknown addr 0x%08x\n", addr);
    exit(1);
}

uint16_t Bus::Read16(uint32_t addr)
{
    if (addr < 0x200000)
        return *(uint16_t*)&bootRom[addr];

    printf("Read16 from unknown addr 0x%08x\n", addr);
    exit(1);
}

uint32_t Bus::Read32(uint32_t addr)
{
    if (addr < 0x200000)
        return *(uint32_t*)&bootRom[addr];
    
    printf("Read32 from unknown addr 0x%08x\n", addr);
    exit(1);
}

void Bus::Write8(uint32_t addr, uint8_t data)
{
    printf("Write8 0x%02x to unknown addr 0x%08x\n", data, addr);
    exit(1);
}

void Bus::Write16(uint32_t addr, uint16_t data)
{
    printf("Write16 0x%04x to unknown addr 0x%08x\n", data, addr);
    exit(1);
}

void Bus::Write32(uint32_t addr, uint32_t data)
{
    printf("Write32 0x%08x to unknown addr 0x%08x\n", data, addr);
    exit(1);
}
