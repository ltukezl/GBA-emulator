#ifndef LOGICALOPS_H
#define LOGICALOPS_H

void And(int& saveTo, int operand1, int operand2);
void Ands(int& saveTo, int operand1, int operand2);
void Eor(int& saveTo, int operand1, int operand2);
void Eors(int& saveTo, int operand1, int operand2);
void Tst(int& saveTo, int operand1, int operand2);
void Teq(int& saveTo, int operand1, int operand2);
void Cmp(int& saveTo, int operand1, int operand2);
void Cmn(int& saveTo, int operand1, int operand2);
void Orr(int& saveTo, int operand1, int operand2);
void Orrs(int& saveTo, int operand1, int operand2);
void Mov(int& saveTo, int operand1, int operand2);
void Movs(int& saveTo, int operand1, int operand2);
void Bic(int& saveTo, int operand1, int operand2);
void Bics(int& saveTo, int operand1, int operand2);
void Mvn(int& saveTo, int operand1, int operand2);
void Mvns(int& saveTo, int operand1, int operand2);

#endif