#pragma once

#include <stdint.h>
#include <memory>
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
};

enum TCPUMode {
	EThumb = 4,
	EArm = 8
};


//-------------------------------
constexpr uint8_t REG_LEN = 17;

class Registers {
private:
	enum CpuModes_r {
		USRr = 0x10,
		FIQq = 0x11,
		IRQq = 0x12,
		SUPERr = 0x13,
		ABORTt = 0x17,
		UNDEFf = 0x1B,
		SYSs = 0x1F
	};


	union CPSR_r {
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
	}cpsr_r;

	TCPUMode m_offset;
    uint32_t** r;

	uint32_t sharedRegs[9];
	uint32_t extRegisters[5];
	uint32_t usrBanked[2];
	uint32_t fiqBanked[7];
	uint32_t svcBanked[2];
	uint32_t abtBanked[2];
	uint32_t irqBanked[2];
	uint32_t undBanked[2];

	uint32_t sprs_usr = 0;
	uint32_t sprs_svc = 0;
	uint32_t sprs_abt = 0;
	uint32_t sprs_irq = 0;
	uint32_t sprs_fiq = 0;
	uint32_t sprs_udf = 0;

	//__int32 r[16];	//used register

	/*prepare complete banks for modes*/
	uint32_t* usrSys[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &usrBanked[0], &usrBanked[1], &sharedRegs[8], &sprs_usr };

	uint32_t* svc[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &svcBanked[0], &svcBanked[1], &sharedRegs[8], &sprs_svc };

	uint32_t* abt[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &abtBanked[0], &abtBanked[1], &sharedRegs[8], &sprs_abt };

	uint32_t* fiq[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&fiqBanked[0], &fiqBanked[1], &fiqBanked[2], &fiqBanked[3], &fiqBanked[4], &fiqBanked[5], &fiqBanked[6], &sharedRegs[8], &sprs_fiq };

	uint32_t* irq[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &irqBanked[0], &irqBanked[1], &sharedRegs[8], &sprs_irq };

	uint32_t* undef[REG_LEN] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
	&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &undBanked[0], &undBanked[1], &sharedRegs[8], &sprs_udf };

public:	
	uint32_t currentMode = 0;

	void updateMode(uint32_t mode) {
		//std::cout << "switched mode to " << mode << std::endl;
		currentMode = mode;
		// FIXME: use returns!!!
		switch (mode) {
			case USR:    r = usrSys;  break;
			case FIQ:    r = fiq;  	  break;
			case IRQ:    r = irq;  	  break;
			case SUPER:  r = svc;  	  break;
			case ABORT:  r = abt;  	  break;
			case UNDEF:  r = undef;   break;
			case SYS:    r = usrSys;  break;
		}
	}

	void reset(const uint32_t mode) {
		updateMode(mode);
		memset(sharedRegs, 0, sizeof(sharedRegs));
		memset(extRegisters, 0, sizeof(extRegisters));
		memset(usrBanked, 0, sizeof(usrBanked));        
		memset(fiqBanked, 0, sizeof(fiqBanked));        
		memset(svcBanked, 0, sizeof(svcBanked));        
		memset(abtBanked, 0, sizeof(abtBanked));        
		memset(irqBanked, 0, sizeof(irqBanked));        
		memset(undBanked, 0, sizeof(undBanked));
	};

	Registers(const uint32_t mode = SUPER) {
		reset(mode);
	};

	Registers(const std::initializer_list<uint32_t> list, const uint32_t mode = SUPER) {
		reset(mode);
		for (uint32_t i = 0; i < REG_LEN; i++) {
			*r[i] = list.begin()[i];
		}
	};

	~Registers() = default;

    uint32_t& operator[](const uint32_t index) const {
        return *r[index];
    }

    bool operator==(const Registers& other) const {
		for (uint8_t i = 0; i < REG_LEN; i++) {
            if (*r[i] != *other.r[i]) {
                return false;
            }
        }
        return true;
    }

	void nextProgramCounter() {
		*r[EProgramCounter] += m_offset;
	}

	void prevProgramCounter() {
		*r[EProgramCounter] -= m_offset;
	}

	/*
	string representation() {
		
		return "Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, USR));"
	}
	*/
};