#include "MemoryOps.h"
#include "DMA.h"
#include "memoryMappedIO.h"
#include "MemoryOps.h"
#include "iostream"


void doDMA(uint32_t i, uint32_t destinationAddress, uint32_t sourceAddress, uint32_t wordCount){
	for (uint32_t k = 0; k < wordCount; k++){
		if (DMAcontrol.transferType == 0){
			writeToAddress16(destinationAddress & ~0x1, loadFromAddress16(sourceAddress & ~0x1));
		}
		else{
			writeToAddress32(destinationAddress & ~0x3, loadFromAddress32(sourceAddress & ~0x3));
		}

		if (DMAcontrol.destCtrl == 0 || DMAcontrol.destCtrl == 3)
			destinationAddress += DMAcontrol.transferType ? 4 : 2;
		else if (DMAcontrol.destCtrl == 1)
			destinationAddress -= DMAcontrol.transferType ? 4 : 2;

		if (DMAcontrol.sourceCtrl == 0)
			sourceAddress += DMAcontrol.transferType ? 4 : 2;
		else if (DMAcontrol.sourceCtrl == 1)
			sourceAddress -= DMAcontrol.transferType ? 4 : 2;
	}
	if (DMAcontrol.irq){
		InterruptFlagRegister.addr = rawLoad16(IoRAM, 0x202);
		if (InterruptEnableRegister.DMA0 && i == 0)
			InterruptFlagRegister.DMA0 = 1;
		else if (InterruptEnableRegister.DMA1 && i == 1)
			InterruptFlagRegister.DMA1 = 1;
		else if (InterruptEnableRegister.DMA2 && i == 2)
			InterruptFlagRegister.DMA2 = 1;
		else if (InterruptEnableRegister.DMA3 && i == 3)
			InterruptFlagRegister.DMA3 = 1;
		rawWrite16(IoRAM, 0x202, InterruptFlagRegister.addr);
	}
}

void startDMA(){
	InterruptFlagRegister.addr = rawLoad16(IoRAM, 0x202);
	for (int i = 0; i < 4; i++){
		DMAcontrol.addr = rawLoad32(IoRAM, 0xBA + 0xC * i);
		if ((DMAcontrol.enable) || (InterruptFlagRegister.hBlank && DMAcontrol.irq == 2) || (InterruptFlagRegister.vBlank && DMAcontrol.irq == 1)){
			uint32_t sourceAddress = rawLoad32(IoRAM, 0xB0 + 0xC * i);
			uint32_t destinationAddress = rawLoad32(IoRAM, 0xB4 + 0xC * i);
			uint32_t wordCount = rawLoad16(IoRAM, 0xB8 + 0xC * i);

			if (sourceAddress < 0x05000000)
				sourceAddress &= 0x7FFFFFF;
			else
				sourceAddress &= 0xFFFFFFF;

			if (destinationAddress < 0x05000000)
				destinationAddress &= 0x7FFFFFF;
			else
				destinationAddress &= 0xFFFFFFF;


			if (i == 3 && wordCount == 0)
				wordCount = 0x10000;
			else if (wordCount == 0)
				wordCount = 0x4000;

			if ((i == 1 || i == 2) && DMAcontrol.timing == 3){
				for (int words = 0; words < 4; words++){
					writeToAddress32(destinationAddress + 4, loadFromAddress32(sourceAddress)); //fifo mode, do addr control logic
				}
			}

			else if (i == 3 && DMAcontrol.timing == 3){
				std::cout << "not used";
			}

			else if (DMAcontrol.timing == 0){
				doDMA(i, destinationAddress, sourceAddress, wordCount);
			}

			else if (DMAcontrol.timing == 1 && InterruptFlagRegister.vBlank){
				doDMA(i, destinationAddress, sourceAddress, wordCount);
			}

			else if (DMAcontrol.timing == 2 && InterruptFlagRegister.hBlank){
				doDMA(i, destinationAddress, sourceAddress, wordCount);
			}
			
			if (!DMAcontrol.repeat) {
				DMAcontrol.enable = 0;
				writeToAddress16(0x40000BA + 0xC * i, DMAcontrol.addr);
			}
			else{
				writeToAddress32(0x40000B4 + 0xC * i, destinationAddress);
			}
		}
	}
}