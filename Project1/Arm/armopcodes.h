#ifndef ARMOPCODES
#define ARMOPCODES

#include <stdio.h>

uint32_t RORnoCond(uint32_t immediate, uint32_t by);

void lslCond(int &saveTo, int from, int immidiate);
void lsrCond(int &saveTo, int from, int immidiate);
void asrCond(int &saveTo, int from, int immidiate);
void rorCond(int &saveTo, int from, int immidiate);

void lslNoCond(int &saveTo, int from, int immidiate);
void lsrNoCond(int &saveTo, int from, int immidiate);
void asrNoCond(int &saveTo, int from, int immidiate);
void rorNoCond(int &saveTo, int from, int immidiate);

void ARMExecute(int);

#endif // ARMOPCODES

