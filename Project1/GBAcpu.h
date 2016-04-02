#ifndef GBACPU_H
#define GBACPU_H
#include <stdint.h>


extern __int32* usrSys[16];
extern __int32* fiq[16];
extern __int32* abt[16];
extern __int32* undef[16];
extern __int32* irq[16];
extern __int32* sup[16];

extern __int32** r;
extern __int32 cprs;
extern __int32* &LR;
extern __int32* &PC;
extern __int32* &SP;

template<int start>
int signExtend(int value){
	int m = 1U << (start - 1); //bitextend hack
	int r = (value ^ m) - m;
	return r;
}

#endif // MAIN_H

