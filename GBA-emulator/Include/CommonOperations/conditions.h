#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <string>

int BEQ();
int BNE();
int BCS();
int BCC();
int BMI();
int BPL();
int BVS();
int BVC();
int BHI();
int BLS();
int BLT();
int BGE();
int BGT();
int BLE();
int noCond();

extern int(*conditions[15])();
extern std::string conditions_s[15];

#endif