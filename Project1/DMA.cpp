#include "MemoryOps.h"
#include "DMA.h"
#include "memoryMappedIO.h"
#include "MemoryOps.h"
#include "iostream"

void doDMA(uint32_t i, uint32_t destinationAddress, uint32_t sourceAddress, uint32_t wordCount){
	for (int k = 0; k < wordCount; k++){
		if (DMAcontrol.transferType == 0){
			writeToAddress16(destinationAddress, loadFromAddress16(sourceAddress));
		}
		else{
			writeToAddress32(destinationAddress, loadFromAddress32(sourceAddress));
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

	if (InterruptEnableRegister.DMA0 && i == 0 && DMAcontrol.irq)
		InterruptFlagRegister.DMA0 = 1;
	else if (InterruptEnableRegister.DMA1 && i == 1 && DMAcontrol.irq)
		InterruptFlagRegister.DMA1 = 1;
	else if (InterruptEnableRegister.DMA2 && i == 2 && DMAcontrol.irq)
		InterruptFlagRegister.DMA2 = 1;
	else if (InterruptEnableRegister.DMA3 && i == 3 && DMAcontrol.irq)
		InterruptFlagRegister.DMA3 = 1;
}

void startDMA(){
	for (int i = 0; i < 4; i++){
		DMAcontrol.addr = loadFromAddress32(0x40000BA + 0xC * i);
		if (DMAcontrol.enable){
			uint32_t sourceAddress = loadFromAddress32(0x40000B0 + 0xC * i, true);
			uint32_t destinationAddress = loadFromAddress32(0x40000B4 + 0xC * i, true);
			uint32_t wordCount = loadFromAddress16(0x40000B8 + 0xC * i, true);

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