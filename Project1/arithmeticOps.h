#ifndef ARITHMETICOPS_H
#define ARITHMETICOPS_H

void negative(int result);

void zero(int result);

void addCarry(int operand1, int operand2, int result);

void addOverflow(int operand1, int operand2, int result);

void subCarry(int operand1, int operand2, int result);

void subOverflow(int operand1, int operand2, int result);

void Sub(int& saveTo, int operand1, int operand2);

void Subs(int& saveTo, int operand1, int operand2);

void Add(int& saveTo, int operand1, int operand2);

void Adds(int& saveTo, int operand1, int operand2);

void Rsb(int& saveTo, int operand1, int operand2);

void Rsbs(int& saveTo, int operand1, int operand2);

void Adc(int& saveTo, int operand1, int operand2);

void Adcs(int& saveTo, int operand1, int operand2);

void Sbc(int& saveTo, int operand1, int operand2);

void Sbcs(int& saveTo, int operand1, int operand2);

void Rsc(int& saveTo, int operand1, int operand2);

void Rscs(int& saveTo, int operand1, int operand2);

void Neg(int& saveTo, int operand1, int operand2);

#endif