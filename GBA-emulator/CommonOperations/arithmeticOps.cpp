#include "GBAcpu.h"

void negative(int result)
{
	r.m_cpsr.negative = result < 0;
}

void zero(int result)
{
	r.m_cpsr.zero = result == 0;
}

void addCarry(int operand1, int operand2, int result)
{
	r.m_cpsr.carry = ((operand1 & operand2) | (operand1 & ~result) | (operand2 & ~result)) >> 31 & 1;
}

void addOverflow(int operand1, int operand2, int result)
{
	r.m_cpsr.overflow = ((operand1 & operand2 & ~result) | (~operand1 & ~operand2 & result)) >> 31 & 1;
}

void subCarry(int operand1, int operand2, int result)
{
	r.m_cpsr.carry = ((operand1 & ~operand2) | (operand1 & ~result) | (~operand2 & ~result)) >> 31 & 1;
}

void subOverflow(int operand1, int operand2, int result)
{
	r.m_cpsr.overflow = ((~operand1 & operand2 & result) | (operand1 & ~operand2 & ~result)) >> 31 & 1;
}

void Sub(int& saveTo, int operand1, int operand2){
	saveTo = operand1 - operand2;
}

void Subs(int& saveTo, int operand1, int operand2){
	saveTo = operand1 - operand2;
	zero(saveTo);
	negative(saveTo);
	subCarry(operand1, operand2, saveTo);
	subOverflow(operand1, operand2, saveTo);
}

void Add(int& saveTo, int operand1, int operand2){
	saveTo = operand1 + operand2;
}

void Adds(int& saveTo, int operand1, int operand2){
	saveTo = operand1 + operand2;
	zero(saveTo);
	negative(saveTo);
	addCarry(operand2, operand1, saveTo);
	addOverflow(operand2, operand1, saveTo);
}

void Rsb(int& saveTo, int operand1, int operand2){
	saveTo = operand2 - operand1;
}

void Rsbs(int& saveTo, int operand1, int operand2){
	saveTo = operand2 - operand1;
	zero(saveTo);
	negative(saveTo);
	subCarry(operand2, operand1, saveTo);
	subOverflow(operand2, operand1, saveTo);
}

void Adc(int& saveTo, int operand1, int operand2){
	saveTo = operand1 + operand2 + r.m_cpsr.carry;
}

void Adcs(int& saveTo, int operand1, int operand2){
	saveTo = operand1 + operand2 + r.m_cpsr.carry;
	zero(saveTo);
	negative(saveTo);
	addCarry(operand2, operand1, saveTo);
	addOverflow(operand2, operand1, saveTo);
}

void Sbc(int& saveTo, int operand1, int operand2){
	saveTo = operand1 - operand2 - !r.m_cpsr.carry;
}

void Sbcs(int& saveTo, int operand1, int operand2){
	saveTo = operand1 - operand2 - !r.m_cpsr.carry;
	zero(saveTo);
	negative(saveTo);
	subCarry(operand1, operand2, saveTo);
	subOverflow(operand1, operand2, saveTo);
}

void Rsc(int& saveTo, int operand1, int operand2){
	saveTo = operand2 - operand1 - !r.m_cpsr.carry;
}

void Rscs(int& saveTo, int operand1, int operand2){
	saveTo = operand2 - operand1 - !r.m_cpsr.carry;
	zero(saveTo);
	negative(saveTo);
	subCarry(operand2, operand1, saveTo);
	subOverflow(operand2, operand1, saveTo);
}

void Neg(int& saveTo, int operand1, int operand2){
	Rsbs(saveTo, operand2, 0);
}