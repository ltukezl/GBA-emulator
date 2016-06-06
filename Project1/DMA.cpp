#include "MemoryOps.h"
#include "DMA.h"
#include "iostream"
//  32   32   32    32  == 128b DMA
//  B0   B4   B8    BA   
//| --------------------- |
//| SAD | DAD | CNT | CTR |
//| --------------------- |

//DMA source address
#define DMA0SAD  (IoRAM[0xB0] | IoRAM[0xB1] << 8 | IoRAM[0xB2] << 16 | IoRAM[0xB3] << 24)
#define DMA1SAD  (IoRAM[0xBC] | IoRAM[0xBD] << 8 | IoRAM[0xBE] << 16 | IoRAM[0xBF] << 24)
#define DMA2SAD  (IoRAM[0xC8] | IoRAM[0xC9] << 8 | IoRAM[0xCA] << 16 | IoRAM[0xCB] << 24)
#define DMA3SAD  (IoRAM[0xD4] | IoRAM[0xD5] << 8 | IoRAM[0xD6] << 16 | IoRAM[0xD7] << 24)

//DMA destination address
#define DMA0DAD  (IoRAM[0xB4] | IoRAM[0xB5] << 8 | IoRAM[0xB6] << 16 | IoRAM[0xB7] << 24)
#define DMA1DAD  (IoRAM[0xC0] | IoRAM[0xC1] << 8 | IoRAM[0xC2] << 16 | IoRAM[0xC3] << 24)
#define DMA2DAD  (IoRAM[0xCC] | IoRAM[0xCD] << 8 | IoRAM[0xCE] << 16 | IoRAM[0xCF] << 24)
#define DMA3DAD  (IoRAM[0xD8] | IoRAM[0xD9] << 8 | IoRAM[0xDA] << 16 | IoRAM[0xDB] << 24)

//DMA word count
#define DMA0CNT  (IoRAM[0xB8] | IoRAM[0xB9] << 8)
#define DMA1CNT  (IoRAM[0xC4] | IoRAM[0xC5] << 8)
#define DMA2CNT  (IoRAM[0xD0] | IoRAM[0xD1] << 8)
#define DMA3CNT  (IoRAM[0xDC] | IoRAM[0xDD] << 8)

//control register
#define DMA0CTR  (IoRAM[0xBA] | IoRAM[0xBB] << 8)
#define DMA1CTR  (IoRAM[0xC6] | IoRAM[0xC7] << 8)
#define DMA2CTR  (IoRAM[0xD2] | IoRAM[0xD3] << 8)
#define DMA3CTR  (IoRAM[0xDE] | IoRAM[0xDF] << 8)

void startDMA(){
	std::cout << DMA3CTR << " ";
	if (DMA3CTR & 0x8000){
		int source = DMA3SAD;
		int dest = DMA3DAD;
		int type = (DMA3CTR >> 10) & 1;
		std::cout << source << " " << dest << " " << type << std::endl;
		for (int i = 0; i < DMA3CNT; i++){
			int valueAt = type ? loadFromAddress32(source) : loadFromAddress16(source);
			type ? writeToAddress32(dest, valueAt) : writeToAddress16(dest, valueAt);
			source += type ? 4 : 2;
		}
		IoRAM[0xDF] &= ~0x80;
	}
	
}