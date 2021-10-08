@echo off
cls
g++ -Wall -ID:\LAB_ALIF\mlib -DCONF_DEBUG_X86ASSM=1 heap_dbg.c x86_assm.cpp -static-libgcc -static-libstdc++ -o test.exe
