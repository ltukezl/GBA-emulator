#pragma once

#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/tests/testUtils.h"
#include <iostream>

struct dataProcessingTest {
	char* name;
	uint32_t opCode;
	Registers registersIn;
	uint32_t cpsrIn;
	ExpectedReg expected;
};

static Registers mockReg;

struct dataProcessingTest carryTests[]{
	{
		"adc 0, 0, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x1F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x4000001F
		}
	},
	{
		"sbc 0, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x1F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x1F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"sbc 0, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"rsc 0, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"adc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x0000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"sbc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x0000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x0000001F,
		{
			Registers({ 0x7FFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"adc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"sbc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"adc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"rsc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"adc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"rsc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
		},
	{
		"sbc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x80000000,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x7000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"adc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"rsc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"adc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"rsc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x7000001F
		}
	},
	{
		"rsc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"sbc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 1,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"rsc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x7000001F
		}
	},
	{
		"sbc 0x80000000, 0x8000000 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0x80000000, 0x80000000 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 1,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"sbc 0x80000000, 0x8000000 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"rsc 0x80000000, 0x80000000 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"adc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"sbc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"adc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"sbc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x4000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x0000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x9000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x3000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x2000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR),
		0x2000001F,
		{
			Registers({ 0,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR), 0x6000001F
		}
	},
};

struct dataProcessingTest PCShifterTests[]{
	{
		"pc",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, true, ImmediateRotater(mockReg,0, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl #0",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10201,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl #1",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 1).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10202,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSL, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10205,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 0, LSL, 1).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10206,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr #0",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 0, LSR, 0).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10201,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 0, LSR, 1).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSR, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10205,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSR, 0).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10204,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x1F
		}
	},
	{
		"add r0, pc, r1, asr #0",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200 + 0x80000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, asr #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ASR, 1).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200 + 0xC0000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, asr r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 0,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10204 + 0x80000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, asr r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10204 + 0xC0000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ROR, 1).m_val),
		Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200 + 0xD0000002,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ROR, 0).m_val),
		Registers({ 0,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10200 + 0xA0000009,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ROR, 0).m_val),
		Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR),
		0x1F,
		{
			Registers({ 0x10204 + 0xD0000002,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR), 0x8000001F
		}
	},
};

void testDataprocessingCarries() {
	bool failed = false;
	for (auto& test_arr : carryTests) {
		bool thisLoop = false;
		union CPSR cpsrin;
		cpsrin.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(cpsrin, test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (cpsrin.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (thisLoop)
			std::cout << "\n";
	}
	assert(!failed);
}

void testDataprocessingPCShifts() {
	bool failed = false;
	for (auto& test_arr: PCShifterTests) {
		union CPSR cpsrin;
		cpsrin.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(cpsrin, test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
		}
		if (cpsrin.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsrin.val << " expected: " << test_arr.expected.cpsr << "\n\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void dataProcessingTests() {
	testDataprocessingPCShifts();
	testDataprocessingCarries();
}
