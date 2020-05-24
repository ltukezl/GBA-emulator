#ifndef GBACPU_H
#define GBACPU_H
#include <stdint.h>


extern __int32* usrSys[17];
extern __int32* abt[17];
extern __int32* undef[17];
extern __int32* irq[17];
extern __int32* svc[17];

extern __int32** r;
extern __int32 cprs;

extern __int64 cycles;
extern __int8 Wait0_N_cycles;
extern __int8 Wait0_S_cycles;

extern bool debug;

template<int start>
int signExtend(int value){
	int m = 1U << (start - 1); //bitextend hack
	int r = (value ^ m) - m;
	return r;
}

#endif // MAIN_H

