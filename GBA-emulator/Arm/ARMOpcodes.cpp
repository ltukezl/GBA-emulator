#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

#include "Arm/armopcodes.h"
#include "Arm/ArmOpcodes/Branch.hpp"
#include "Arm/ArmOpcodes/multiply.hpp"
#include "Arm/ArmOpcodes/SingleDataTransferImmediate.hpp"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/logicalOps.h"
#include "Constants.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "GBAcpu.h"
#include "Interrupt/interrupt.h"
#include "Memory/MemoryOps.h"

void incrementBase(int& baseRegister, bool nullParameter = false){
	baseRegister += 4;
}

void decrementBase(int& baseRegister, bool nullParameter = false){
	baseRegister -= 4;
}

template <typename function1, typename function2>
void BlockDataTransferSave(int opCode, function1 a, function2 b){
	__int16 baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	int usrMode = (opCode >> 22) & 1;
	__int16 regList = opCode & 0xFFFF;
	int oldBase = r[baseReg];

	auto currentMode = r.getMode();

	if (usrMode && !((opCode >> 15) & 1)){
		r.updateMode(CpuModes_t::EUSR);
	}

	if (debug)
		std::cout << "push ";

	if (((opCode >> 15) & 1) & ~upDownBit){
		a((int&)r[baseReg], r[15] + 8);
		b((int&)r[baseReg], r[15] + 8);
	}

	for (int i = 0; i < 15; i++){
		if (upDownBit){
			if (regList & 1){
				if (i == 13 && baseReg == 13){
					a((int&)r[baseReg], oldBase);
					b((int&)r[baseReg], oldBase);
				}
				else{
					a((int&)r[baseReg], r[i]);
					b((int&)r[baseReg], r[i]);
				}
				if (debug)
					std::cout << "r" << i << " ";
			}
			regList >>= 1;
		}
		//we are pushing from higer registers first
		else if (~upDownBit){
			if (regList & 0x4000){
				if (i == 1 && baseReg == 13){
					a((int&)r[baseReg], oldBase);
					b((int&)r[baseReg], oldBase);
				}
				else{
					a((int&)r[baseReg], r[14 - i]);
					b((int&)r[baseReg], r[14 - i]);
				}
				if (debug)
					std::cout << "r" << 14 - i << " ";
			}
			regList <<= 1;
		}
	}

	if (((opCode >> 15) & 1) & upDownBit){
		a((int&)r[baseReg], r[15] + 8);
		b((int&)r[baseReg], r[15] + 8);
	}

	if (usrMode){
		r.updateMode(currentMode);
	}

	r[baseReg] = writeBack ? r[baseReg] : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPost(int opCode, function1 a, function2 b){ // not tested for r15
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	int usrMode = (opCode >> 22) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = r[baseReg];
	int memAddress = oldBase & ~0x3;
	bool baseInRList = regList & (1 << baseReg);

	auto currentMode = r.getMode();

	if (regList == 0){
		r[PC] = loadFromAddress32(r[baseReg]);
		r[baseReg] += 0x40;
		return;
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r.updateMode(CpuModes_t::EUSR);
	}

	if (debug)
		std::cout << "pop ";

	for (int i = 0; i < 16; i++){
		if (upDownBit){
			if (regList & 1){
				r[i] = a(memAddress, false);
				b(memAddress, false);
				if (debug)
					std::cout << "r" << i << " ";
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x8000){
				r[15 - i] = a(memAddress, false);
				b(memAddress, false);
				if (debug)
					std::cout << "r" << 14 - i << " ";
			}
			regList <<= 1;
		}
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r.updateMode(currentMode);
	}

	if (!baseInRList)
		r[baseReg] = writeBack ? memAddress : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPre(int opCode, function1 a, function2 b){
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	bool usrMode = (opCode >> 20) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = r[baseReg];
	int memAddress = oldBase & ~0x3;
	bool baseInRList = regList & (1 << baseReg);

	auto currentMode = r.getMode();

	if (regList == 0){
		r[PC] = loadFromAddress32(r[baseReg]);
		r[baseReg] += 0x40;
		return;
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r.updateMode(CpuModes_t::EUSR);
	}

	for (int i = 0; i < 16; i++){
		if (upDownBit){
			if (regList & 1){
				a(memAddress, false);
				r[i] = b(memAddress, false);
				if (i == 15)
					r[16] = r.m_cpsr.val;
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x8000){
				a(memAddress, false);
				r[15 - i] = b(memAddress, false);
				if (i == 0)
					r[16] = r.m_cpsr.val;
			}
			regList <<= 1;
		}
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r.updateMode(currentMode);
	}

	if (!baseInRList)
		r[baseReg] = writeBack ? memAddress : oldBase;
}


void singleDataSwap(int opCode){
	uint32_t rm = opCode & 0xF;
	uint32_t rd = (opCode >> 12) & 0xF;
	uint32_t rn = (opCode >> 16) & 0xF;
	bool byteFlag = (opCode >> 22) & 1;

	uint32_t tmp = byteFlag ? loadFromAddress(r[rn]) : loadFromAddress32(r[rn]);
	byteFlag ? writeToAddress(r[rn], r[rm]) : writeToAddress32(r[rn], r[rm]);
	r[rd] = tmp;

	if (debug)
		std::cout << "swp r" << +rd << " r" << +rm << " r[" << +rn << "] ";
}

void lslCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;

	if (immidiate > 32)
		r.m_cpsr.carry = 0;
	else if (immidiate > 0)
		r.m_cpsr.carry = ((unsigned)tmp >> (32 - immidiate) & 1);
	negative(saveTo);
	zero(saveTo);
}

void lsrCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;

	if (immidiate > 0)
		r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
	negative(saveTo);
	zero(saveTo);
}

void asrCond(int &saveTo, int from, int immidiate) {
	int64_t tmp = from;
	saveTo = tmp >> immidiate;

	if (immidiate != 0)
		r.m_cpsr.carry = (tmp >> (immidiate - 1) & 1);
	zero(saveTo);
	negative(saveTo);
}

void rorCond(int &saveTo, int from, int immidiate){
	if (immidiate > 32){
		rorCond(saveTo, from, immidiate - 32);
	}
	else{
		if (immidiate > 0)
			r.m_cpsr.carry = (from >> (immidiate - 1) & 1);
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
		negative(saveTo);
		zero(saveTo);
	}
}

void lslNoCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;
}

void lsrNoCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;
}

void asrNoCond(int &saveTo, int from, int immidiate) {
	int64_t tmp = from;
	saveTo = tmp >> immidiate;
}

void rorNoCond(int &saveTo, int from, int immidiate){
	if (immidiate > 32){
		rorNoCond(saveTo, from, immidiate - 32);
	}
	else{
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
	}
}

uint32_t RORnoCond(uint32_t immediate, uint32_t by){
	if (by > 32){
		RORnoCond(immediate, by - 32);
	}
	return (immediate >> by) | (immediate << (32 - by));
}

void rrx(int& saveTo, uint32_t from, bool conditions){
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

void updateMode(){
	//std::cout << "switched mode to " << mode << std::endl;
	switch (r.m_cpsr.mode){
	case USR:
		r.updateMode(CpuModes_t::EUSR);
		break;
	case FIQ:
		r.updateMode(CpuModes_t::EFIQ);
		break;
	case IRQ:
		r.updateMode(CpuModes_t::EIRQ);
		break;
	case SUPER:
		r.updateMode(CpuModes_t::ESUPER);
		break;
	case ABORT:
		r.updateMode(CpuModes_t::EABORT);
		break;
	case UNDEF:
		r.updateMode(CpuModes_t::EUNDEF);
		break;
	case SYS:
		r.updateMode(CpuModes_t::ESYS);
		break;
	}
}

void msr(int& saveTo, int operand1, int operand2){
	r.m_cpsr.val = operand2;
	updateMode();
}

void msr2(int& saveTo, int operand1, int operand2){
	r[16] = operand2;
	updateMode();
}

void MSR(uint32_t opCode){
	bool SPSR = (opCode >> 22) & 1;
	CPSR_t tmp_cpsr;
	uint8_t rotate = (opCode >> 8) & 0xF;
	uint32_t imm = opCode & 0xFF;
	uint32_t shiftedImm = RORnoCond(imm, rotate);
	shiftedImm = RORnoCond(shiftedImm, rotate);
	tmp_cpsr.val = shiftedImm;

	if (SPSR)
		r[16] = shiftedImm;
	else {
		r.m_cpsr.zero = tmp_cpsr.zero;
		r.m_cpsr.overflow = tmp_cpsr.overflow;
		r.m_cpsr.carry = tmp_cpsr.carry;
		r.m_cpsr.negative = tmp_cpsr.negative;
	}
}


void mrs(int& saveTo, int operand1, int operand2){
	saveTo = r.m_cpsr.val;
}

void mrs2(int& saveTo, int operand1, int operand2){
	saveTo = r[16];
}

void(*dataOperations[0x20])(int&, int, int) = { And, Ands, Eor, Eors, Sub, Subs, Rsb, Rsbs,
Add, Adds, Adc, Adcs, Sbc, Sbcs, Rsc, Rscs, mrs, Tst, msr, Teq, mrs2,
Cmp, msr2, Cmn, Orr, Orrs, Mov, Movs, Bic, Bics, Mvn, Mvns };

std::string dataOperations_s[0x20] = { "and", "ands", "or", "ors", "sub", "subs", "rsb", "rsbs",
"add", "adds", "adc", "adcs", "sbc", "sbcs", "rsc", "rscs", "tst", "tst", "msr", "teq", "cmp",
"cmp", "msr", "cmn", "or", "ors", "mov", "movs", "bic", "bics", "mvn", "mvns" };

void immediateRotate(int opCode){
	bool codeExecuted = false;
	if (((opCode >> 12) & 0x3FF) == 0x28f && ((opCode >> 23) & 3) == 2 && ((opCode >> 26) & 3) == 0 && !codeExecuted)
	{
		int sprs = (opCode >> 22) & 1;
		int rm = opCode & 0xF;

		if (sprs){
			int tmp = r.m_cpsr.val & 0xFFFFFFF;
			tmp |= r[rm] & 0xF0000000;
			r[16] = tmp;
		}
		else{
			int tmp = r.m_cpsr.val & 0xFFFFFFF;
			tmp |= r[rm] & 0xF0000000;
			r.m_cpsr.val = tmp;
		}
		codeExecuted = true;
	}

	if (!codeExecuted) {
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

		if (shiftId == 3 && immediate == 0){
			rrx(tmpRegister, tmpRegister, conditions);
		}
		else{
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

		if (rd == 15 && (opCode >> 20) & 1){
			r.m_cpsr.val = r[16];
			updateMode();
		}

		if (debug)
			std::cout << dataOperations_s[operationID] << " r" << +rd << ", r" << +rs << ", r" << +rn << ", " << ARMshifts_s[shiftId] << " =" << +immediate << " ";
	}
}

void registerRotate(int opCode){
	int rd = (opCode >> 12) & 0xF; //destination
	int rn = (opCode >> 16) & 0xF; //first operand
	int operand = opCode & 0xFF; //2nd operand bytes
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

	if (rd == TRegisters::EProgramCounter && (opCode >> 20) & 1){
		r.m_cpsr.val = r[16];
		updateMode();
	}

	if (rm == 15)
		r[rm] -= 8;
	else if (rn == 15)
		r[rn] -= 8;

	if (debug)
		std::cout << dataOperations_s[operationID] << " r" << +rd << ", r" << +rn << ", r" << +rm << " " << ARMshifts_s[shiftId] << " " << +operand << " ";

}

void dataProcessingImmediate(int opCode){
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
	
	if (rd == 15 && conditions){
		r.m_cpsr.val = r[16];
		updateMode();
	}

	if (debug)
		std::cout << dataOperations_s[operationID] << " r" << rd << ", r" << rs << ", " << result << " ";
}

void halfDataTransfer(int opCode){
	int pFlag = (opCode >> 24) & 1;
	int uFlag = (opCode >> 23) & 1;
	int func = (opCode >> 22) & 1;
	int wFlag = (opCode >> 21) & 1;
	int lFlag = (opCode >> 20) & 1;
	int shFlag = (opCode >> 5) & 3;
	int rn = (opCode >> 16) & 0xF;
	int rd = (opCode >> 12) & 0xF;
	int offset = (opCode >> 4) & 0xF0 | opCode & 0xF;
	offset += (rn == 15) ? 8 : 0;
	int calculated = (rd == 15) ? (r[rn] + 8) : r[rn];

	switch (func){
	case 0:
		if (lFlag){
			if (pFlag)
				calculated += uFlag ? r[offset] : -r[offset];
			if (shFlag == 1)
				r[rd] = loadFromAddress16(calculated);
			else if (shFlag == 2)
				r[rd] = signExtend<8>(loadFromAddress(calculated));
			else
				if (calculated & 1){
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
		else{
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
		if (lFlag){
			if (pFlag)
				calculated += uFlag ? offset : -offset;
			if (shFlag == 1)
				r[rd] = loadFromAddress16(calculated);
			else if (shFlag == 2)
				r[rd] = signExtend<8>(loadFromAddress(calculated));
			else {
				uint32_t res = loadFromAddress16(calculated);
				res = signExtend<16>(res);
				if (calculated & 1) {
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
		else{
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

void singleDataTrasnferRegisterPre(int opCode){
	int offset = 0;

	int rn = (opCode >> 16) & 0xF;
	int rd = (opCode >> 12) & 0xF;
	int rm = opCode & 0xF;

	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int writeBack = (opCode >> 21) & 1;
	int loadStore = (opCode >> 20) & 1;

	const auto func = BarrelShifterDecoder::decode(opCode);
	const int tmpRegister = func(r, opCode, 0);

	int oldReg = r[rn];
	switch (loadStore){
	case 0:
		r[rn] += upDownBit ? tmpRegister : -tmpRegister;
		byteFlag ? writeToAddress(r[rn], r[rd]) : writeToAddress32(r[rn], r[rd]);
		r[rn] = (writeBack) ? r[rn] : oldReg;
		break;

	case 1:
		r[rn] += upDownBit ? tmpRegister : -tmpRegister;
		auto ret = byteFlag ? loadFromAddress(r[rn]) : loadFromAddress32(r[rn]);
		r[rn] = (writeBack) ? r[rn] : oldReg;
		r[rd] = ret;

		if (debug && byteFlag)
			std::cout << "ldrb r" << +rd << ", [r" << +rn << " r" << +rm << "] ";
		else if (debug && !byteFlag)
			std::cout << "ldr r" << +rd << ", [r" << +rn << " r" << +rm << "] ";
		break;
	}

	cycles += S_cycles + N_cycles + 1;
}

void singleDataTrasnferRegisterPost(int opCode){
	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int writeback = (opCode >> 21) & 1;
	int loadStore = (opCode >> 20) & 1;
	int baseReg = (opCode >> 16) & 15;
	int destinationReg = (opCode >> 12) & 15;

	const auto func = BarrelShifterDecoder::decode(opCode);
	const int tmpRegister = func(r, opCode, 0);

	int oldReg = r[baseReg];

	switch (loadStore){
	case 0:
		byteFlag ? writeToAddress(r[baseReg], r[destinationReg]) : writeToAddress32(r[baseReg], r[destinationReg]);
		r[baseReg] += upDownBit ? tmpRegister : -tmpRegister;
		if (destinationReg == 15)
			r[destinationReg] -= 8;
		break;
	case 1:
		const auto ret = byteFlag ? loadFromAddress(r[baseReg]) : loadFromAddress32(r[baseReg]);
		r[baseReg] += upDownBit ? tmpRegister : -tmpRegister;
		r[destinationReg] = ret;
		break;
	}

	cycles += S_cycles + N_cycles + 1;
}

static uint32_t constexpr reduce_opcode(uint32_t opCode)
{
	return ((opCode >> 20) & 0x1F);
}

template<uint32_t opCode>
decltype(&SingleDataTransfer::SingleDataTransferIPrDWNS::execute) constexpr populate_func()
{
	if constexpr (SingleDataTransfer::SingleDataTransferIPrDWNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDWNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDBNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDBNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUBNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUBNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUWNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUWNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDWWS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDWWS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDBWS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDBWS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUBWS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUBWS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUWWS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUWWS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDWNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDWNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDBNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDBNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUBNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUBNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUWNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUWNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDWWL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDWWL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrDBWL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrDBWL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPrUBWL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPrUBWL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoUWNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoUWNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoUBNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoUBNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoDWNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoDWNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoDBNS::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoDBNS::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoUWNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoUWNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoUBNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoUBNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoDWNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoDWNL::execute;
	}
	else if constexpr (SingleDataTransfer::SingleDataTransferIPoDBNL::isThisOpcode(opCode))
	{
		return &SingleDataTransfer::SingleDataTransferIPoDBNL::execute;
	}
	else
	{
		return &SingleDataTransfer::SingleDataTransferIPrUWWL::execute;
	}
}


template<uint32_t baseOp, typename T>
consteval void insert_opcodes(T& arr) {
	insert_opcodes_impl<baseOp>(arr, std::make_index_sequence<64>{});
}

template<uint32_t baseOp, typename T, std::size_t... Is>
consteval void insert_opcodes_impl(T& arr, std::index_sequence<Is...>) {
	((arr[Is] = populate_func<(baseOp + static_cast<uint32_t>(Is) * 0x10'0000)>()), ...);
}

static constexpr std::array m_dispatch_table = { []() consteval {
	std::array<decltype(&SingleDataTransfer::SingleDataTransferIPrDWNS::execute), 64> tmp {};
	constexpr uint32_t start = 0x400'0000;
	insert_opcodes<start>(tmp);
	return tmp;
}() };

void ARMExecute(int opCode){
	int condition = (opCode >> 28) & 0xF;
	cycles += 1;
	//units[ProcessingUnits::EDataProcessing] = new DataProcessingOpcode(cpsr, Registers());
	if (conditions[condition]()) //condition true
	{
		if (((opCode >> 25) & 0x7) == 2)
		{
			m_dispatch_table[reduce_opcode(opCode)](r, opCode);
			// std::println("{}", SingleDataTransfer::disassemble(opCode));
			return;
		}

		if (branches::ArmBranch::isThisOpcode(opCode))
		{
			branches::ArmBranch::execute(r, opCode);
			// std::println("{}", branches::ArmBranch::disassemble(opCode));
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
		int subType;
		switch (opCodeType){
		case 15:
			interruptController();
			break;
		case 14: //coProcessor data ops / register transfer, not used in GBA
			break;
		case 13: case 12: //co processor data transfer, not used in GBA
			break;
		case 9: //block data transfer pre offset. maybe implement S bits
			subType = (opCode >> 20) & 0xB;
			switch (subType){
			case 10: case 8: //writeback / no writeback, pre offset, add offset
				BlockDataTransferSave(opCode, incrementBase, writeToAddress32);
				break;
			case 2: case 0: //writeback / no writeback, pre offset, sub offset
				BlockDataTransferSave(opCode, decrementBase, writeToAddress32);
				break;
			case 11: case 9: //writeback / no writeback, post offset, add offset
				BlockDataTransferLoadPre(opCode, incrementBase, loadFromAddress32);
				break;
			case 3: case 1: //writeback / no writeback, post offset, sub offset
				BlockDataTransferLoadPre(opCode, decrementBase, loadFromAddress32);
				break;
			}
			break;
		case 8: //block data transfer post offset
			subType = (opCode >> 20) & 0xB;
			switch (subType){
			case 10: case 8: //writeback / no writeback, post offset, add offset
				BlockDataTransferSave(opCode, writeToAddress32, incrementBase);
				break;
			case 2: case 0: //writeback / no writeback, post offset, sub offset
				BlockDataTransferSave(opCode, writeToAddress32, decrementBase);
				break;
			case 11: case 9: //writeback / no writeback, post offset, add offset
				BlockDataTransferLoadPost(opCode, loadFromAddress32, incrementBase);
				break;
			case 3: case 1: //writeback / no writeback, post offset, sub offset
				BlockDataTransferLoadPost(opCode, loadFromAddress32, decrementBase);
				break;
			}
			break;
		case 7:// single data transfer, register pre offset 
			singleDataTrasnferRegisterPre(opCode);
			break;
		case 6:// single data transfer, register, post offset
			singleDataTrasnferRegisterPost(opCode);
			break;
		case 5:// single data transfer, immediate pre offset
			break;
		case 4: // single data transfer, immediate post offset
			break;
		case 3: case 2: //data processing, immediate check msr?
			if ((((opCode >> 12) & 0x3FF) == 0x28F) && (((opCode >> 23) & 0x3) == 2) && (((opCode >> 26) & 0x3) == 0)) {
				MSR(opCode);//<-
				//units[ProcessingUnits::EDataProcessing]->execute(opCode);
			}
			else {
				dataProcessingImmediate(opCode);
				//units[ProcessingUnits::EDataProcessing]->execute(opCode);
			}
			break;
		case 1: case 0: //data prceossing, multiply, data transfer, branch and exhange
			if (((opCode >> 4) & 1) == 0){ //data processing
				//units[ProcessingUnits::EDataProcessing]->execute(opCode);
				immediateRotate(opCode);//<-
			}
			else if (((opCode >> 7) & 1) == 0){ //data processing
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
