#ifndef TOC_H
#define TOC_H
void negative(int result);
void zero(int result);
void addCarry(int operand1, int operand2, int result);
void addOverflow(int operand1, int operand2, int result);
void subCarry(int operand1, int operand2, int result);
void subOverflow(int operand1, int operand2, int result);

void lsl(int&, int, int);
void lsr(int&, int, int);
void asr(int&, int, int);

void add(int&, int, int);
void sub(int&, int, int);

void mov(int&, int);
void cmp(int&, int);
void add8imm(int&, int);
void sub8imm(int&, int);

void TAND(int&, int);
void TEOR(int&, int);
void lslip(int&, int);
void lsrip(int&, int);
void asrip(int&, int);
void adc(int&, int);
void sbc(int&, int);
void rorIP(int&, int);
void tst(int&, int);
void neg(int&, int);
void cmpReg(int&, int);
void cmnReg(int&, int);
void ORR(int&, int);
void mul(int&, int);
void bic(int&, int);
void mvn(int&, int);

void addNoCond(int&, int);
void movNoCond(int&, int);
void cmpHL(int&, int);
void bx(int&, int);

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

int thumbExecute(short);
#endif
