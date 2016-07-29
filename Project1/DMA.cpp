#include "MemoryOps.h"
#include "DMA.h"
#include "iostream"
//  32   32   16    16  == 96b DMA
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

	int DMARegisters[4][4] = {{ DMA0SAD, DMA0DAD, DMA0CNT, DMA0CTR },  
							  { DMA1SAD, DMA1DAD, DMA1CNT, DMA1CTR },
							  { DMA2SAD, DMA2DAD, DMA2CNT, DMA2CTR },
							  { DMA3SAD, DMA3DAD, DMA3CNT, DMA3CTR }};
	//prioritises 0 to 3 first
	for (int i = 0; i < 4; i++){
		int source		= DMARegisters[i][0];
		int dest		= DMARegisters[i][1];
		int byteCount	= DMARegisters[i][2];
		int control		= DMARegisters[i][3];

		if (control & 0x8000){
			int type = (control >> 10) & 1;

			for (int k = 0; k < byteCount; k++){
				int valueAt = type ? loadFromAddress32(source) : loadFromAddress16(source);
				type ? writeToAddress32(dest, valueAt) : writeToAddress16(dest, valueAt);
				source += (type ? 4 : 2); //either 32 bit mode or 16 bit mode
				dest += (type ? 4 : 2);
			}
			IoRAM[0xBB + i * 0xC] = ((control >> 9) & 1) ? IoRAM[0xBB + i * 0xC] : IoRAM[0xBB + i * 0xC] & ~0x80;
		}
	}
	

	
}