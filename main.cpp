#include "bus.h"
#include "cpu.h"

CPU* sh4;

void Dump()
{
    sh4->Dump();
}

int main()
{
    Bus::LoadRoms("dc_boot.bin", "dc_flash.bin");

    sh4 = new CPU();
    sh4->Reset();

    std::atexit(Dump);

    while (1)
        sh4->Clock();
}