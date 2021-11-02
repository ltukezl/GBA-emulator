#include "GBAcpu.h"
#include "logicalOps.h"
#include "arithmeticOps.h"

void And(int& saveTo, int operand1, int operand2){
	saveTo = operand1 & operand2;
}

void Ands(int& saveTo, int operand1, int operand2){
	saveTo = operand1 & operand2;
	zero(saveTo);
	negative(saveTo);
}

void Eor(int& saveTo, int operand1, int operand2){
	saveTo = operand1 ^ operand2;
}

void Eors(int& saveTo, int operand1, int operand2){
	saveTo = operand1 ^ operand2;
	zero(saveTo);
	negative(saveTo);
}

void Tst(int& saveTo, int operand1, int operand2){
	zero(operand1 & operand2);
	negative(operand1 & operand2);
}

void Teq(int& saveTo, int operand1, int operand2){
	zero(operand1 ^ operand2);
	negative(operand1 ^ operand2);
}

void Cmp(int& saveTo, int operand1, int operand2){
	zero(operand1 - operand2);
	negative(operand1 - operand2);
	subCarry(operand1, operand2, operand1 - operand2);
	subOverflow(operand1, operand2, operand1 - operand2);
}

void Cmn(int& saveTo, int operand1, int operand2){
	zero(operand1 + operand2);
	negative(operand1 + operand2);
	addCarry(operand1, operand2, operand1 + operand2);
	addOverflow(operand1, operand2, operand1 + operand2);
}

void Orr(int& saveTo, int operand1, int operand2){
	saveTo = operand1 | operand2;
}

void Orrs(int& saveTo, int operand1, int operand2){
	saveTo = operand1 | operand2;
	zero(saveTo);
	negative(saveTo);
}

void Mov(int& saveTo, int operand1, int operand2){
	saveTo = operand2;
}

void Movs(int& saveTo, int operand1, int operand2){
	saveTo = operand2;
	zero(saveTo);
	negative(saveTo);
}

void Bic(int& saveTo, int operand1, int operand2){
	saveTo = operand1 & ~operand2;
}

void Bics(int& saveTo, int operand1, int operand2){
	saveTo = operand1 & ~operand2;
	zero(saveTo);
	negative(saveTo);
}

void Mvn(int& saveTo, int operand1, int operand2){
	saveTo = ~operand2;
}

void Mvns(int& saveTo, int operand1, int operand2){
	saveTo = ~operand2;
	zero(saveTo);
	negative(saveTo);
}