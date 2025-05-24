#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

//-------------------------------

enum TRegisters {
	ER0 = 0,
	ER1,
	ER2,
	ER3,
	ER4,
	ER5,
	ER6,
	ER7,
	ER8,
	ER9,
	ER10,
	EFramePointer,
	EIP,
	EStackPointer,
	ELinkRegisterLR,
	EProgramCounter,
	ESavedStatusRegister,
	ECPSR,
};

enum CpuModes_t {
	EUSR = 0x10,
	EFIQ = 0x11,
	EIRQ = 0x12,
	ESUPER = 0x13,
	EABORT = 0x17,
	EUNDEF = 0x1B,
	ESYS = 0x1F
};

enum TCPUMode {
	EThumb = 4,
	EArm = 8
};

union CPSR_t {
	struct {
		uint32_t mode : 5;
		uint32_t thumb : 1;
		uint32_t FIQDisable : 1;
		uint32_t IRQDisable : 1;
		uint32_t unused : 20;
		uint32_t overflow : 1;
		uint32_t carry : 1;
		uint32_t zero : 1;
		uint32_t negative : 1;
	};
	uint32_t val;

	void updateAll(uint32_t newVal) {
		if (mode == EUSR) {
			updateFlags(newVal);
		}
		else {
			val = newVal;
		}
	}

	void updateFlags(uint32_t newVal) {
		CPSR_t tmp = {};
		tmp.val = newVal;
		negative = tmp.negative;
		zero = tmp.zero;
		carry = tmp.carry;
		overflow = tmp.overflow;
	}

	void saveAll(uint32_t& dest) {
		if (mode == EUSR) {
			saveFlags(dest);
		}
		else {
			dest = val;
		}
	}

	void saveFlags(uint32_t& dest) {
		uint32_t tmp = val & 0xF000'0000;
		dest = tmp;
	}
};


//-------------------------------
constexpr uint8_t REG_LEN = 17;

class Registers {
private:
	TCPUMode m_offset = EArm;
	std::array<uint32_t*, REG_LEN> const * r;
	uint32_t m_previousMode = ESYS;

	std::array<uint32_t, 9> sharedRegs;
	std::array<uint32_t, 5> extRegisters;
	std::array<uint32_t, 2> usrBanked;
	std::array<uint32_t, 7> fiqBanked;
	std::array<uint32_t, 2> svcBanked;
	std::array<uint32_t, 2> abtBanked;
	std::array<uint32_t, 2> irqBanked;
	std::array<uint32_t, 2> undBanked;

	uint32_t sprs_usr = 0;
	uint32_t sprs_svc = 0;
	uint32_t sprs_abt = 0;
	uint32_t sprs_irq = 0;
	uint32_t sprs_fiq = 0;
	uint32_t sprs_udf = 0;

	/*prepare complete banks for modes*/
	const std::array<uint32_t*, REG_LEN> usrSys = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &usrBanked[0], &usrBanked[1], &sharedRegs[8], &sprs_usr };

	const std::array<uint32_t*, REG_LEN> svc = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &svcBanked[0], &svcBanked[1], &sharedRegs[8], &sprs_svc };

	const std::array<uint32_t*, REG_LEN> abt = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &abtBanked[0], &abtBanked[1], &sharedRegs[8], &sprs_abt };

	const std::array<uint32_t*, REG_LEN> fiq = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&fiqBanked[0], &fiqBanked[1], &fiqBanked[2], &fiqBanked[3], &fiqBanked[4], &fiqBanked[5], &fiqBanked[6], &sharedRegs[8], &sprs_fiq };

	const std::array<uint32_t*, REG_LEN> irq = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &irqBanked[0], &irqBanked[1], &sharedRegs[8], &sprs_irq };

	const std::array<uint32_t*, REG_LEN> undef = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &undBanked[0], &undBanked[1], &sharedRegs[8], &sprs_udf };

public:	

	union CPSR_t m_cpsr;

	void updateMode(const CpuModes_t mode) {
		m_previousMode = m_cpsr.mode;
		m_cpsr.mode = mode;
		// FIXME: use returns!!!
		switch (mode) {
			case EUSR:    r = &usrSys;  break;
			case EFIQ:    r = &fiq;     break;
			case EIRQ:    r = &irq;     break;
			case ESUPER:  r = &svc;     break;
			case EABORT:  r = &abt;     break;
			case EUNDEF:  r = &undef;   break;
			case ESYS:    r = &usrSys;  break;
		}
	}

	auto getMode() const {
		return static_cast<CpuModes_t>(m_cpsr.mode);
	}

	auto getPreviousMode() const {
		return static_cast<CpuModes_t>(m_previousMode);
	}

	void reset(const CpuModes_t mode) {
		updateMode(mode);
		sharedRegs.fill(0);
		extRegisters.fill(0);
		usrBanked.fill(0);
		fiqBanked.fill(0);
		svcBanked.fill(0);
		abtBanked.fill(0);
		irqBanked.fill(0);
		undBanked.fill(0);
	};

	Registers(const CpuModes_t mode = ESUPER) {
		reset(mode);
	};

	Registers(const std::initializer_list<uint32_t> list, const CpuModes_t mode = ESUPER) {
		reset(mode);
		std::copy(list.begin(), list.end(), *r->begin());
	};

    uint32_t& operator[](const uint32_t index) const {
        return *(*r)[index];
    }

    bool operator==(const Registers& other) const {
		for (uint8_t i = 0; i < REG_LEN; i++) {
            if (*(*r)[i] != *(*other.r)[i]) {
                return false;
            }
        }
        return true;
    }

	bool operator!=(const Registers& other) const {
		return !(* this == other);
	}

	void nextProgramCounter() {
		*(*r)[EProgramCounter] += m_offset;
	}

	void prevProgramCounter() {
		*(*r)[EProgramCounter] -= m_offset;
	}

	/*
	string representation() {
		
		return "Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, USR));"
	}
	*/
};