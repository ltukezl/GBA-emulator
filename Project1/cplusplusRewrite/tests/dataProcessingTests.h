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
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x4000001F
		}
	},
	{
		"sbc 0, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"sbc 0, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"rsc 0, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"adc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"sbc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0x7FFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"adc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"sbc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"adc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"rsc 0, 0x80000000, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"adc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"rsc 0, 0x80000000, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
		},
	{
		"sbc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x80000000,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x7FFFFFFF, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x7FFFFFFF, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x80000000, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x7000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0x80000000, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"adc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"sbc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"rsc 0x7FFFFFFF, 0xFFFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"adc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"rsc 0x80000000, 0 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"adc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"rsc 0x80000000, 0 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x7000001F
		}
	},
	{
		"rsc 0x80000000, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"sbc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 1,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"rsc 0x80000000, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"adc 0x80000000, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x7000001F
		}
	},
	{
		"sbc 0x80000000, 0x8000000 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0x80000000, 0x80000000 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 1,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"sbc 0x80000000, 0x8000000 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"rsc 0x80000000, 0x80000000 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0x80000000  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"adc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"sbc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"adc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"sbc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0x80000000, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0x80000000,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0xFFFFFFFF   ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0, !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x4000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0, c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x7FFFFFFF , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x7FFFFFFF , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x7FFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x9000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x3000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x7FFFFFFE,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x80000000 , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000000,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x7FFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0x80000000 , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0x80000001,0x80000000 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFE,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0xFFFFFFFF  , !c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"adc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(ADC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000001F
		}
	},
	{
		"sbc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(SBC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
	{
		"rsc 0xFFFFFFFF, 0xFFFFFFFF  , c",
		DataProcessingOpcode::fromFields(RSC, SET, ER0, ER0, false, RegisterWithImmediateShifter(mockReg,ER1, LSL, 0).m_val),
		Registers({ 0xFFFFFFFF,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x2000001F,
		{
			Registers({ 0,0xFFFFFFFF  ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x6000001F
		}
	},
};

struct dataProcessingTest PCShifterTests[]{
	{
		"pc",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, true, ImmediateRotater(mockReg,0, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl #0",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10201,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl #1",
		DataProcessingOpcode::fromFields(ADD, SET, ER0, EProgramCounter, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 1).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10202,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSL, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10205,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsl r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 0, LSL, 1).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10206,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr #0",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 0, LSR, 0).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 0, LSR, 1).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSR, 0).m_val),
		Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10205,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, lsr r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, LSR, 0).m_val),
		Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10204,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x1F
		}
	},
	{
		"add r0, pc, r1, asr #0",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200 - 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x2000001F
		}
	},
	{
		"add r0, pc, r1, asr #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ASR, 1).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200 + 0xC0000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, asr r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 0,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10204 + 0x80000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, asr r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ASR, 0).m_val),
		Registers({ 1,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10204 + 0xC0000000,0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror #1",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithImmediateShifter(mockReg, 1, ROR, 1).m_val),
		Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200 + 0xD0000002,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror r0 (0)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ROR, 0).m_val),
		Registers({ 0,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10200 + 0xA0000009,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
	{
		"add r0, pc, r1, ror r0 (1)",
		DataProcessingOpcode::fromFields(ADD, SET, 0, 15, false, RegisterWithRegisterShifter(mockReg, 1, ROR, 0).m_val),
		Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR),
		0x1F,
		{
			Registers({ 0x10204 + 0xD0000002,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, EUSR), 0x8000001F
		}
	},
};

struct dataProcessingTest tstTests[]{
	{
		"tst r0 (0xFFFF), #0",
		DataProcessingOpcode::fromFields(TST, SET, ER1, ER0, true, ImmediateRotater(mockReg, 0,0).m_val),
		Registers({ 0xFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x3000001F,
		{
			Registers({ 0xFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x7000'001F
		}
	},
	{
		"tst r0 (0xFFFF), #0",
		DataProcessingOpcode::fromFields(TST, SET, ER1, ER0, true, ImmediateRotater(mockReg, 0,0).m_val),
		Registers({ 0xFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0xFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x4000'001F
		}
	},
	{
		"tst r0 (0x80000000), #FF000000",
		DataProcessingOpcode::fromFields(TST, SET, ER1, ER0, true, ImmediateRotater(mockReg, 0xFF,8).m_val),
		Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0x80000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xA000'001F
		}
	},
	{
		"tst r0 (0xFFFF), r2 (1) lsl r3 (1)",
		DataProcessingOpcode::fromFields(TST, SET, ER1, ER0, false, RegisterWithRegisterShifter(mockReg, ER3, LSL, ER2).m_val),
		Registers({ 0xFFFF,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x0000001F,
		{
			Registers({ 0xFFFF,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x0000'001F
		}
	},
	{
		"tst r0 (0xFFFF), r2 (1) lsl r3 (1)",
		DataProcessingOpcode::fromFields(TST, SET, ER1, ER0, false, RegisterWithRegisterShifter(mockReg, ER3, LSL, ER2).m_val),
		Registers({ 0xFFFF,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x10000001F,
		{
			Registers({ 0xFFFF,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x1000'0001F
		}
	},
};

struct dataProcessingTest psrTests[]{
	{
		"mrs r0 cpsr",
		DataProcessingOpcode::fromFields(TST, NO_SET, ER0, EProgramCounter, false, 0),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x50000010,
		{
			Registers({ 0x50000010,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x50000010
		}
	},
	{
		"msr cpsr_all, r4, usrMode",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER9, false, 4),
		Registers({ 0,0,0,0,0x50000000,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x010,
		{
			Registers({ 0,0,0,0,0x50000000,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x50000010
		}
	},
	{
		"msr cpsr_all, r4, usrMode, masked",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER9, false, 4),
		Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x010,
		{
			Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x50000010
		}
	},
	{
		"msr cpsr_flags, r4, usrMode",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER8, false, 4),
		Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x010,
		{
			Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x50000010
		}
	},
	{
		"msr cpsr_flags, 0xF000'0000, usrMode",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER8, true, ImmediateRotater(mockReg, 0xF, 4).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR),
		0x010,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0xF0000010
		}
	},
	{
		"msr cpsr_all, r4, privilidged",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER9, false, 4),
		Registers({ 0,0,0,0,0x5000001F,0,0,0,0,0,0,0,0,0,0,0,0 }, EIRQ),
		0x12,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EUSR), 0x5000001F
		}
	},
	{
		"msr cpsr_flags, r4, privilidged",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER8, false, 4),
		Registers({ 0,0,0,0,0x5000001F,0,0,0,0,0,0,0,0,0,0,0,0 }, EIRQ),
		0x12,
		{
			Registers({ 0,0,0,0,0x5000001F,0,0,0,0,0,0,0,0,0,0,0,0 }, EIRQ), 0x50000012
		}
	},
	{
		"msr cpsr_flags, 0x3000'0000, privilidged",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, ER8, true, ImmediateRotater(mockReg, 0x3, 4).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EIRQ),
		0x012,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, EIRQ), 0x30000012
		}
	},
	{
		"msr spsr_all, r4",
		DataProcessingOpcode::fromFields(CMN, NO_SET, EProgramCounter, ER9, false, 4),
		Registers({ 0,0,0,0,0x5000001F,0,0,0,0,0,0,0,0,0,0,0,0xA0000012 }, EIRQ),
		0x50000012,
		{
			Registers({ 0,0,0,0,0x5000001F,0,0,0,0,0,0,0,0,0,0,0,0x5000001F }, EIRQ), 0x50000012
		}
	},
	{
		"msr spsr_flags, r4",
		DataProcessingOpcode::fromFields(CMN, NO_SET, EProgramCounter, ER8, false, 4),
		Registers({ 0,0,0,0,0x50000000,0,0,0,0,0,0,0,0,0,0,0,0xA0000012 }, EIRQ),
		0x50000012,
		{
			Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0x50000012 }, EIRQ), 0x50000012
		}
	},
	{
		"msr spsr_flags, #7",
		DataProcessingOpcode::fromFields(CMN, NO_SET, EProgramCounter, ER8, true, ImmediateRotater(mockReg, 7, 4).m_val),
		Registers({ 0,0,0,0,0x50000000,0,0,0,0,0,0,0,0,0,0,0,0xA0000012 }, EIRQ),
		0x50000012,
		{
			Registers({ 0,0,0,0,0x50000012,0,0,0,0,0,0,0,0,0,0,0,0x70000012 }, EIRQ), 0x50000012
		}
	},
	{
		"mrs r0 spsr_all",
		DataProcessingOpcode::fromFields(CMP, NO_SET, ER0, EProgramCounter, false, 0),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA0000012 }, EIRQ),
		0x50000012,
		{
			Registers({ 0xA0000012,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA0000012 }, EIRQ), 0x50000012
		}
	},
	{
		"MOV pc #0x8",
		DataProcessingOpcode::fromFields(MOV, SET, EProgramCounter, ER0, true, ImmediateRotater(mockReg, 8, 0).m_val),
		Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ),
		0x50000012,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0 }, EUSR), 0xA000001f
		}
	},
	{
		"add pc, r0, r1",
		DataProcessingOpcode::fromFields(ADD, SET, EProgramCounter, ER0, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ),
		0x50000012,
		{
			Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0 }, EUSR), 0xA000001f
		}
	},
	{
		"add pc, r0, r1",
		DataProcessingOpcode::fromFields(ADD, NO_SET, EProgramCounter, ER0, false, RegisterWithImmediateShifter(mockReg, ER1, LSL, 0).m_val),
		Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ),
		0x50000012,
		{
			Registers({ 0xA0000012,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0xA000001f }, EIRQ), 0x50000012
		}
	},
	{
		"swp r0, r1",
		DataProcessingOpcode::fromFields(TST, NO_SET, ELinkRegisterLR, ER0, false, 0x90),
		Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ),
		0x50000012,
		{
			Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ), 0x50000012
		}
	},
	{
		"bx r1",
		DataProcessingOpcode::fromFields(TEQ, NO_SET, EProgramCounter, EProgramCounter, false, RegisterWithRegisterShifter(mockReg, EProgramCounter, LSL, ER0).m_val),
		Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ),
		0x50000012,
		{
			Registers({ 4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xA000001f }, EIRQ), 0x50000012
		}
	},
};
void testDataprocessingCarries() {
	bool failed = false;
	for (auto& test_arr : carryTests) {
		bool thisLoop = false;
		test_arr.registersIn.m_cpsr.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (test_arr.registersIn.m_cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << test_arr.registersIn.m_cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
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
		test_arr.registersIn.m_cpsr.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
		}
		if (test_arr.registersIn.m_cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << test_arr.registersIn.m_cpsr.val << " expected: " << test_arr.expected.cpsr << "\n\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void tstOperandTests() {
	bool failed = false;
	for (auto& test_arr : tstTests) {
		bool thisLoop = false;
		test_arr.registersIn.m_cpsr.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (test_arr.registersIn.m_cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << test_arr.registersIn.m_cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (thisLoop)
			std::cout << "\n";
	}
	assert(!failed);
}

void psrOperandTests() {
	bool failed = false;
	for (auto& test_arr : psrTests) {
		bool thisLoop = false;
		test_arr.registersIn.m_cpsr.val = test_arr.cpsrIn;
		auto DataProcessing = DataProcessingOpcode(test_arr.registersIn);
		DataProcessing.execute(test_arr.opCode);

		if (test_arr.registersIn != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << test_arr.registersIn[ER0] << " expected: " << test_arr.expected.result[ER0] << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (test_arr.registersIn.m_cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << test_arr.registersIn.m_cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
			thisLoop = true;
		}
		if (thisLoop)
			std::cout << "\n";
	}
	assert(!failed);
}

void dataProcessingTests() {
	testDataprocessingPCShifts();
	testDataprocessingCarries();
	tstOperandTests();
	psrOperandTests();
}
