#include "bus.h"
#include "cpu.h"
#include <csignal>

CPU* sh4;

void Dump()
{
    sh4->Dump();
    Bus::Dump();
}

void sig(int)
{
    exit(1);
}

int main()
{
    Bus::LoadRoms("dc_boot.bin", "dc_flash.bin");

    sh4 = new CPU();
    sh4->Reset();

    std::atexit(Dump);
    std::signal(SIGINT, sig);

    while (1)
        sh4->Clock();
}