bool MathOp::isZero(uint32_t result){ return result == 0; }
bool MathOp::isNegative(int32_t result){ return result < 0; }
bool MathOp::additionCarry(uint64_t op1, uint64_t op2, uint64_t carry){
	uint32_t res = op1 + op2 + carry;
	return ((res >> 32) & 1) == 1;
}
bool MathOp::additionOverflow(uint64_t op1, uint64_t op2, uint64_t carry){
	return cpsr.overflow = ((op1 & op2 & ~carry) | (~op1 & ~op2 & carry)) >> 31 & 1;
}
bool MathOp::substractionBorrow(uint64_t op1, uint64_t op2, uint64_t carry){
	return ((op1 & ~op2) | (op1 & ~carry) | (~op2 & ~carry)) >> 31 & 1;
}
bool MathOp::substractionUnderflow(uint64_t op1, uint64_t op2, uint64_t carry){
	return ((~op1 & op2 & carry) | (op1 & ~op2 & ~carry)) >> 31 & 1;
}