#ifndef TOC_H
#define TOC_H

int thumbExecute(uint16_t);

union moveShiftedRegisterOp{
	uint16_t op;
	struct{
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t immediate : 5;
		uint16_t instruction : 2;
	};
};

union addSubRegisterOp{
	uint16_t op;
	struct{
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t regOrImmediate : 3;
		uint16_t Sub : 1;
		uint16_t useImmediate : 1;
	};
};

union movCmpAddSub{
	uint16_t op;
	struct{
		uint16_t offset : 8;
		uint16_t destination : 3;
		uint16_t instruction : 2;
	};
};

union aluOps{
	uint16_t op;
	struct{
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t instruction : 4;
	};
};

union hiRegOps{
	uint16_t op;
	struct{
		uint16_t destination : 3;
		uint16_t source : 4;
		uint16_t destHiBit : 1;
		uint16_t instruction : 2;
	};
};

union PCRelativeLoad{
	uint16_t op;
	struct{
		uint16_t offset : 8;
		uint16_t destination : 3;
	};
};

union loadStoreRegOffset{
	uint16_t op;
	struct{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offsetReg : 3;
		uint16_t: 1;
		uint16_t byteSize : 1;
		uint16_t loadBit : 1;
	};
};

union loadStoreSignExtended{
	uint16_t op;
	struct{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offsetReg : 3;
		uint16_t: 1;
		uint16_t extend : 1;
		uint16_t halfWord : 1;
	};
};

union loadStoreImmediate{
	uint16_t op;
	struct{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offset : 5;
		uint16_t loadFlag : 1;
		uint16_t byteSize : 1;
	};
};

union loadStoreHalfWord{
	uint16_t op;
	struct{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offset : 5;
		uint16_t loadFlag : 1;
	};
};

union SPrelativeLoad{
	uint16_t op;
	struct{
		uint16_t immediate : 8;
		uint16_t destSourceReg : 3;
		uint16_t loadFlag : 1;
	};
};

union loadAddress{
	uint16_t op;
	struct{
		uint16_t immediate : 8;
		uint16_t destination : 3;
		uint16_t useSP : 1;
	};
};

union offsetToStack{
	uint16_t op;
	struct{
		uint16_t immediate : 7;
		uint16_t negative : 1;
	};
};

union pushPopReg{
	uint16_t op;
	struct{
		uint16_t regList : 8;
		uint16_t PCRLBit : 1;
		uint16_t:2;
		uint16_t loadBit : 1;
	};
};

union multiLoadStore{
	uint16_t op;
	struct{
		uint16_t regList : 8;
		uint16_t baseReg : 3;
		uint16_t loadBit : 1;
	};
};

union conditionalBranchOp{
	uint16_t op;
	struct{
		int16_t immediate : 8;
		uint16_t condition : 4;
	};
};

#endif
