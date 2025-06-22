#include <array>
#include <cstdint>
#include <string>
#include <utility>

#include "Arm/armopcodes.h"
#include "Arm/ArmOpcodes/Branch.hpp"
#include "Arm/ArmOpcodes/Multiply.hpp"

#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/logicalOps.h"
#include "Arm/ArmOpcodes/SDDHelper.hpp"
#include "Arm/ArmOpcodes/Undefop.hpp"
#include "Arm/ArmOpcodes/BlockDataTransferLoads.hpp"
#include "Arm/ArmOpcodes/BlockDataTransferStores.hpp"
#include "Constants.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "GBAcpu.h"
#include "Interrupt/interrupt.h"
#include "Memory/memoryOps.h"

static void singleDataSwap(int opCode)
{
	uint32_t rm = opCode & 0xF;
	uint32_t rd = (opCode >> 12) & 0xF;
	uint32_t rn = (opCode >> 16) & 0xF;
	bool byteFlag = (opCode >> 22) & 1;

	uint32_t tmp = byteFlag ? loadFromAddress(r[rn]) : loadFromAddress32(r[rn]);
	byteFlag ? writeToAddress(r[rn], r[rm]) : writeToAddress32(r[rn], r[rm]);
	r[rd] = tmp;
}

void lslCond(int& saveTo, int from, int immidiate)
{
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;

	if (immidiate > 32)
		r.m_cpsr.carry = 0;
	else if (immidiate > 0)
		r.m_cpsr.carry = ((unsigned)tmp >> (32 - immidiate) & 1);
	negative(saveTo);
	zero(saveTo);
}

void lsrCond(int& saveTo, int from, int immidiate)
{
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;

	if (immidiate > 0)
		r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
	negative(saveTo);
	zero(saveTo);
}

void asrCond(int& saveTo, int from, int immidiate)
{
	int64_t tmp = from;
	saveTo = tmp >> immidiate;

	if (immidiate != 0)
		r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
	zero(saveTo);
	negative(saveTo);
}

void rorCond(int& saveTo, int from, int immidiate)
{
	if (immidiate > 32)
	{
		rorCond(saveTo, from, immidiate - 32);
	}
	else
	{
		if (immidiate > 0)
			r.m_cpsr.carry = (from >> (immidiate - 1) & 1);
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
		negative(saveTo);
		zero(saveTo);
	}
}

void lslNoCond(int& saveTo, int from, int immidiate)
{
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;
}

void lsrNoCond(int& saveTo, int from, int immidiate)
{
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;
}

void asrNoCond(int& saveTo, int from, int immidiate)
{
	int64_t tmp = from;
	saveTo = tmp >> immidiate;
}

void rorNoCond(int& saveTo, int from, int immidiate)
{
	if (immidiate > 32)
	{
		rorNoCond(saveTo, from, immidiate - 32);
	}
	else
	{
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
	}
}

uint32_t RORnoCond(uint32_t immediate, uint32_t by)
{
	if (by > 32)
	{
		RORnoCond(immediate, by - 32);
	}
	return (immediate >> by) | (immediate << (32 - by));
}

static void rrx(int& saveTo, uint32_t from, bool conditions)
{
	saveTo = (r.m_cpsr.carry << 31) | (from >> 1);
	if (conditions)
	{
		r.m_cpsr.carry = from & 1;
		zero(saveTo);
		negative(saveTo);
	}
}

void(*ARMshifts[4])(int&, int, int) = { lslCond, lsrCond, asrCond, rorCond };
void(*ARMshiftsNoCond[4])(int&, int, int) = { lslNoCond, lsrNoCond, asrNoCond, rorNoCond };
std::string ARMshifts_s[4] = { "lsl", "lsr", "asr", "ror" };

void updateMode()
{
	//std::cout << "switched mode to " << mode << std::endl;
	switch (r.m_cpsr.mode)
	{
		case CpuModes_t::EUSR:
			r.updateMode(CpuModes_t::EUSR);
			break;
		case CpuModes_t::EFIQ:
			r.updateMode(CpuModes_t::EFIQ);
			break;
		case CpuModes_t::EIRQ:
			r.updateMode(CpuModes_t::EIRQ);
			break;
		case CpuModes_t::ESUPER:
			r.updateMode(CpuModes_t::ESUPER);
			break;
		case CpuModes_t::EABORT:
			r.updateMode(CpuModes_t::EABORT);
			break;
		case CpuModes_t::EUNDEF:
			r.updateMode(CpuModes_t::EUNDEF);
			break;
		case CpuModes_t::ESYS:
			r.updateMode(CpuModes_t::ESYS);
			break;
	}
}

void msr(int& saveTo, int operand1, int operand2)
{
	r.m_cpsr.val = operand2;
	updateMode();
}

void msr2(int& saveTo, int operand1, int operand2)
{
	r[16] = operand2;
	updateMode();
}

void MSR(uint32_t opCode)
{
	bool SPSR = (opCode >> 22) & 1;
	CPSR_t tmp_cpsr{};
	uint8_t rotate = (opCode >> 8) & 0xF;
	uint32_t imm = opCode & 0xFF;
	uint32_t shiftedImm = RORnoCond(imm, rotate);
	shiftedImm = RORnoCond(shiftedImm, rotate);
	tmp_cpsr.val = shiftedImm;

	if (SPSR)
		r[16] = shiftedImm;
	else
	{
		r.m_cpsr.zero = tmp_cpsr.zero;
		r.m_cpsr.overflow = tmp_cpsr.overflow;
		r.m_cpsr.carry = tmp_cpsr.carry;
		r.m_cpsr.negative = tmp_cpsr.negative;
	}
}


void mrs(int& saveTo, int operand1, int operand2)
{
	saveTo = r.m_cpsr.val;
}

void mrs2(int& saveTo, int operand1, int operand2)
{
	saveTo = r[16];
}

void(*dataOperations[0x20])(int&, int, int) = { And, Ands, Eor, Eors, Sub, Subs, Rsb, Rsbs,
Add, Adds, Adc, Adcs, Sbc, Sbcs, Rsc, Rscs, mrs, Tst, msr, Teq, mrs2,
Cmp, msr2, Cmn, Orr, Orrs, Mov, Movs, Bic, Bics, Mvn, Mvns };

std::string dataOperations_s[0x20] = { "and", "ands", "or", "ors", "sub", "subs", "rsb", "rsbs",
"add", "adds", "adc", "adcs", "sbc", "sbcs", "rsc", "rscs", "tst", "tst", "msr", "teq", "cmp",
"cmp", "msr", "cmn", "or", "ors", "mov", "movs", "bic", "bics", "mvn", "mvns" };

void immediateRotate(int opCode)
{
	bool codeExecuted = false;
	if (((opCode >> 12) & 0x3FF) == 0x28f && ((opCode >> 23) & 3) == 2 && ((opCode >> 26) & 3) == 0 && !codeExecuted)
	{
		int sprs = (opCode >> 22) & 1;
		int rm = opCode & 0xF;

		if (sprs)
		{
			int tmp = r.m_cpsr.val & 0xFFFFFFF;
			tmp |= r[rm] & 0xF0000000;
			r[16] = tmp;
		}
		else
		{
			int tmp = r.m_cpsr.val & 0xFFFFFFF;
			tmp |= r[rm] & 0xF0000000;
			r.m_cpsr.val = tmp;
		}
		codeExecuted = true;
	}

	if (!codeExecuted)
	{
		int rd = (opCode >> 12) & 15; //destination
		int rs = (opCode >> 16) & 15; //first operand
		int rn = opCode & 15; //2nd operand
		int tmpRegister = r[rn];
		int immediate = (opCode >> 7) & 0x1F;
		int shiftId = (opCode >> 5) & 3;
		int operationID = (opCode >> 20) & 0x1F;
		int conditions = (opCode >> 20) & 1;
		int operand1 = r[rs];

		if (rn == 15)
			tmpRegister += 4;
		else if (rs == 15)
			operand1 += 4;

		if (shiftId == 3 && immediate == 0)
		{
			rrx(tmpRegister, tmpRegister, conditions);
		}
		else
		{
			if (immediate == 0 && shiftId != 0)
				immediate = 0x20;
			if (conditions && (((operationID > 3) && (operationID < 16)) || ((operationID > 21) && (operationID < 24))))
				ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
			else if (conditions)
				ARMshifts[shiftId](tmpRegister, tmpRegister, immediate);
			else
				ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
		}

		dataOperations[operationID]((int&)r[rd], operand1, tmpRegister);

		if (rd == 15 && (opCode >> 20) & 1)
		{
			r.m_cpsr.val = r[16];
			updateMode();
		}
	}
}

void registerRotate(int opCode)
{
	int rd = (opCode >> 12) & 0xF; //destination
	int rn = (opCode >> 16) & 0xF; //first operand
	int rm = opCode & 0xF;
	int rs = (opCode >> 8) & 0xF;
	int shiftId = (opCode >> 5) & 3;
	int shiftAmount = r[rs] & 0xFF;
	int tmpResult = 0;
	int operationID = (opCode >> 20) & 0x1F;
	int conditions = (opCode >> 20) & 1;

	if (rm == 15)
		r[rm] += 8;
	else if (rn == 15)
		r[rn] += 8;

	if (conditions && (((operationID > 3) && (operationID < 16)) || ((operationID > 21) && (operationID < 24))))
		ARMshiftsNoCond[shiftId](tmpResult, r[rm], shiftAmount);
	else if (conditions)
		ARMshifts[shiftId](tmpResult, r[rm], shiftAmount);
	else
		ARMshiftsNoCond[shiftId](tmpResult, r[rm], shiftAmount);

	dataOperations[operationID]((int&)r[rd], r[rn], tmpResult);

	if (rd == TRegisters::EProgramCounter && (opCode >> 20) & 1)
	{
		r.m_cpsr.val = r[16];
		updateMode();
	}

	if (rm == 15)
		r[rm] -= 8;
	else if (rn == 15)
		r[rn] -= 8;
}

void dataProcessingImmediate(int opCode)
{
	int rd = (opCode >> 12) & 0xF; //destination
	int rs = (opCode >> 16) & 0xF; //first operand
	int operand1 = r[rs];
	if (rs == TRegisters::EProgramCounter)
		operand1 += 4;
	bool conditions = (opCode >> 20) & 1;
	int operationID = (opCode >> 20) & 0x1F;
	int operation = (opCode >> 21) & 0xF;

	const bool isLogicalOp = (operation == 0b0000) || (operation == 0b0001) || (operation == 0b1000) || (operation == 0b1001) || (operation == 0b1100) || (operation == 0b1101) || (operation == 0b1110) || (operation == 0b1111);
	const auto func = BarrelShifterDecoder::decode(opCode);
	const uint32_t result = func(r, opCode, conditions && isLogicalOp);
	dataOperations[operationID]((int&)r[rd], operand1, result);

	if (rd == 15 && conditions)
	{
		r.m_cpsr.val = r[16];
		updateMode();
	}
}

static void halfDataTransfer(int opCode)
{
	int pFlag = (opCode >> 24) & 1;
	int uFlag = (opCode >> 23) & 1;
	int func = (opCode >> 22) & 1;
	int wFlag = (opCode >> 21) & 1;
	int lFlag = (opCode >> 20) & 1;
	int shFlag = (opCode >> 5) & 3;
	int rn = (opCode >> 16) & 0xF;
	int rd = (opCode >> 12) & 0xF;
	int offset = ((opCode >> 4) & 0xF0) | (opCode & 0xF);
	offset += (rn == 15) ? 8 : 0;
	int calculated = (rd == 15) ? (r[rn] + 8) : r[rn];

	switch (func)
	{
		case 0:
			if (lFlag)
			{
				if (pFlag)
					calculated += uFlag ? r[offset] : -r[offset];
				if (shFlag == 1)
					r[rd] = loadFromAddress16(calculated);
				else if (shFlag == 2)
					r[rd] = signExtend<8>(loadFromAddress(calculated));
				else
					if (calculated & 1)
					{
						r[rd] = loadFromAddress16(calculated);
						if (r[rd] & 0x80) //sign bit on
							r[rd] |= 0xFFFFFF00;
						else
							r[rd] &= 0xFFFF;
					}
					else
						r[rd] = signExtend<16>(loadFromAddress16(calculated));
				if (!pFlag)
					calculated += uFlag ? r[offset] : -r[offset];
			}
			else
			{
				if (pFlag)
					calculated += uFlag ? r[offset] : -r[offset];
				if (shFlag == 1)
					writeToAddress16(calculated, r[rd]);
				else if (shFlag == 2)
					writeToAddress(calculated, signExtend<8>(r[rd]));
				else
					writeToAddress16(calculated, signExtend<16>(r[rd]));
				if (!pFlag)
					calculated += uFlag ? r[offset] : -r[offset];
			}
			r[rn] = (wFlag || !pFlag) ? calculated : r[rn];
			break;
		case 1:
			if (lFlag)
			{
				if (pFlag)
					calculated += uFlag ? offset : -offset;
				if (shFlag == 1)
					r[rd] = loadFromAddress16(calculated);
				else if (shFlag == 2)
					r[rd] = signExtend<8>(loadFromAddress(calculated));
				else
				{
					uint32_t res = loadFromAddress16(calculated);
					res = signExtend<16>(res);
					if (calculated & 1)
					{
						if (res & 0x80) //sign bit on
							res |= 0xFFFFFF00;
						else
							res &= 0xFFFF;
					}
					r[rd] = res;
				}
				if (!pFlag)
					calculated += uFlag ? offset : offset;
				if (rn != rd)
					r[rn] = (wFlag || !pFlag) ? calculated : r[rn];
			}
			else
			{
				if (pFlag)
					calculated += uFlag ? offset : -offset;
				if (shFlag == 1)
					writeToAddress16(calculated, r[rd]);
				else if (shFlag == 2)
					writeToAddress(calculated, signExtend<8>(r[rd]));
				else
					writeToAddress16(calculated, signExtend<16>(r[rd]));
				if (!pFlag)
					calculated += uFlag ? offset : -offset;
				r[rn] = (wFlag || !pFlag) ? calculated : r[rn];
			}
			break;
	}
}

static void null_func(Registers&, const uint32_t) {}

static uint32_t constexpr reduce_opcode(const uint32_t opCode)
{
	const uint32_t low_byte = (opCode >> 4) & 0xF;
	const uint32_t high_byte = (opCode >> 16) & 0xFF0;
	return low_byte | high_byte;
}

template<uint32_t opCode>
static auto constexpr decode_arm_opcode()
{
	if constexpr (UndefOp::isThisOpcode(opCode))
		return &UndefOp::execute;
	if constexpr (((opCode >> 26) & 0x3) == 1)
		return SingleDataTransfer::decode_sdd<opCode>();
	if constexpr (BlockDataTransfer::isThisOpcode(opCode))
	{
		if constexpr (BlockDataTransferLoad::isThisOpcode(opCode))
			return BlockDataTransferLoad::execute<BlockDataTransfer::mask(opCode)>;
		if constexpr (BlockDataTransferStore::isThisOpcode(opCode))
			return BlockDataTransferStore::execute<BlockDataTransfer::mask(opCode)>;
	}
	if constexpr (branches::ArmBranch::isThisOpcode(opCode))
		return branches::ArmBranch::execute<branches::ArmBranch::mask(opCode)>;
	return &null_func;
}

static consteval auto index_to_opcode(const uint32_t opcode)
{
	const uint32_t low_bits = opcode & 0xF;
	const uint32_t high_bits = opcode & 0xFF0;
	return (high_bits << 16) | (low_bits << 4);
}

template<typename T, std::size_t... Is>
consteval void insert_opcodes(T& arr, std::index_sequence<Is...>)
{
	((arr[Is] = decode_arm_opcode<index_to_opcode(Is)>()), ...);
}

static constexpr std::array m_dispatch_table = { []() consteval
{
	std::array<void(*)(Registers&, const uint32_t), 0xC00> tmp {};
	insert_opcodes(tmp, std::make_index_sequence<tmp.size()>{});
	return tmp;
}() };

#include <print>

void ARMExecute(int opCode)
{
	int condition = (opCode >> 28) & 0xF;
	cycles += 1;
	//units[ProcessingUnits::EDataProcessing] = new DataProcessingOpcode(cpsr, Registers());
	if (conditions[condition]()) //condition true
	{
		if (((opCode >> 26) & 0x3) == 1)
		{
			m_dispatch_table[reduce_opcode(opCode)](r, opCode);
			// std::println("{}", SingleDataTransfer::disassemble(opCode));
			return;
		}

		if (branches::ArmBranch::isThisOpcode(opCode))
		{
			m_dispatch_table[reduce_opcode(opCode)](r, opCode);
			return;
		}

		if (branches::ArmBranchAndExhange::isThisOpcode(opCode))
		{
			branches::ArmBranchAndExhange::execute(r, opCode);
			// std::println("{}", branches::ArmBranchAndExhange::disassemble(opCode));
			return;
		}

		if (MultiplyAccumulate::isThisOpcode(opCode))
		{
			MultiplyAccumulate::execute(r, opCode);
			// std::println("{}", MultiplyAccumulate::disassemble(opCode));
			return;
		}

		if (MultiplyLong::isThisOpcode(opCode))
		{
			MultiplyLong::execute(r, opCode);
			//std::println("{}", MultiplyLong::disassemble(opCode));
			return;
		}

		int opCodeType = (opCode >> 24) & 0xF;
		switch (opCodeType)
		{
			case 15:
				interruptController();
				break;
			case 14: //coProcessor data ops / register transfer, not used in GBA
				break;
			case 13: case 12: //co processor data transfer, not used in GBA
				break;
			case 9: //block data transfer pre offset. maybe implement S bits
				m_dispatch_table[reduce_opcode(opCode)](r, opCode);
				break;
			case 8: //block data transfer post offset
				m_dispatch_table[reduce_opcode(opCode)](r, opCode);
				break;
			case 5:// single data transfer, immediate pre offset
				break;
			case 4: // single data transfer, immediate post offset
				break;
			case 3: case 2: //data processing, immediate check msr?
				if ((((opCode >> 12) & 0x3FF) == 0x28F) && (((opCode >> 23) & 0x3) == 2) && (((opCode >> 26) & 0x3) == 0))
				{
					MSR(opCode);//<-
					//units[ProcessingUnits::EDataProcessing]->execute(opCode);
				}
				else
				{
					dataProcessingImmediate(opCode);
					//units[ProcessingUnits::EDataProcessing]->execute(opCode);
				}
				break;
			case 1: case 0: //data prceossing, multiply, data transfer, branch and exhange
				if (((opCode >> 4) & 1) == 0)
				{ //data processing
//units[ProcessingUnits::EDataProcessing]->execute(opCode);
					immediateRotate(opCode);//<-
				}
				else if (((opCode >> 7) & 1) == 0)
				{ //data processing
//units[ProcessingUnits::EDataProcessing]->execute(opCode);
					registerRotate(opCode);  //<-
				}
				else if ((((opCode >> 23) & 0x1F) == 2) && (((opCode >> 4) & 0xFF) == 9))
					singleDataSwap(opCode);
				else
					halfDataTransfer(opCode);
				break;
		}
	}
}
