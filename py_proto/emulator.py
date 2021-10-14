"""
This module contains various prototypes to demonstrate
important things.
"""

from dataclasses import dataclass


@dataclass
class CPRS:
    carry: int = 0
    negative: int = 0
    zero: int = 0


@dataclass
class ShiferUnit:
    m_cpsr: CPRS = None     

    def execute(self, sourceValue, shiftAmount, setStatus):
        result = self.shift(sourceValue, shiftAmount)
        if setStatus:
            self.calcConditions(result, sourceValue, shiftAmount)
        return result


class Ror(ShiferUnit):
	def calcConditions(self, result, sourceValue, shiftAmount):
		if sourceValue > 0:
			self.m_cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1)
		self.m_cpsr.negative = result < 0
		self.m_cpsr.zero = result == 0

	def shift(self, sourceValue, shiftAmount):
		if shiftAmount > 32:
			return self.shift(sourceValue, shiftAmount - 32)
		else:
			return (sourceValue >> shiftAmount) | (sourceValue << (32 - shiftAmount))


SHIFTERS = [
    None,
    None,
    None,
    Ror(CPRS())
]


@dataclass
class ImmediateRotater:
    immediate: int      # 8 bits
    rotateAmount: int   # 4 bits
    setStatusCodes: int # 1 bit

    def execute(self):
        temp = SHIFTERS[3].execute(self.immediate, self.rotateAmount, self.setStatusCodes)
        return SHIFTERS[3].execute(temp, self.rotateAmount, self.setStatusCodes)


@dataclass
class Asd:
    sourceRegister: int # 4 bits
    type: int           # 1 bits
    shiftCode: int      # 2 bits
    reminder: int       # 5 bits


@dataclass
class DataProcessingOpcode:
    immediate: int              # 12 bits
    destinationRegister: int    # 4 bits
    firstOperandRegister: int   # 4 bits
    setStatusCodes: int         # 1 bits
    dataProcessingOpcode: int   # 4 bits
    unused: int                 # 2 bits
    executionCondition: int     # 4 bits

    def execute(self):
        first_op = self.firstOperandRegister  # Index to register array (0-15 indexes)
        second_op = self.immediate.execute()
        func = self.dataProcessingOpcode      # Index to function pointer array

        # TODO: Status Code
        self.destinationRegister = func(first_op, second_op)


def create_data_porcessing_op_koodi(op_code):
    if op_code >> 25 & 0x01:
        immediate_bits = op_code & 0xfff

        immediate = ImmediateRotater(
            immediate=immediate_bits & 0xff,
            rotateAmount=immediate_bits >> 8,
            setStatusCodes=op_code >> 21 & 0x01,
            )
    else:
        immediate = Asd()

    op_code = DataProcessingOpcode(
        immediate=immediate,
        destinationRegister=op_code >> 1,
        firstOperandRegister=op_code >> 1,
        setStatusCodes=op_code >> 21 & 0x01,
        dataProcessingOpcode=op_code >> 1,
        unused=op_code >> 1,
        executionCondition=op_code >> 1,        
    )

    return op_code


if __name__ == "__main__":
    data_op = create_data_porcessing_op_koodi(0xffffffff)
    data_op.execute()
