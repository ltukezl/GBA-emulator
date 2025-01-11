#include "DMA/DMA.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"
#include "iostream"

using DMAcontrol_t = DMAcontrol::control_t;
using DMAtransfer_t = DMAcontrol::transferType_t;
using DMAtiming_t = DMAcontrol::timing_t;

void doDMA(DMAcontrol* DMAControl, uint32_t i, MemoryAddress destinationAddress, MemoryAddress sourceAddress, uint32_t wordCount) {
	if ((DMAControl->transferType == DMAtransfer_t::WORD) &&
		(DMAControl->destCtrl == DMAcontrol_t::Increment || DMAControl->destCtrl == DMAcontrol_t::IncOrReload) &&
		(DMAControl->sourceCtrl == DMAcontrol_t::Increment)) {
		DmaIncreasing(i, destinationAddress, sourceAddress, wordCount);
	}
	else {
		for (uint32_t k = 0; k < wordCount; k++) {
			if (DMAControl->transferType == DMAtransfer_t::HALFWORD) {
				if (sourceAddress.mask > EGamePak6 && i == 0) {
					writeToAddress16(destinationAddress.raw & (~1), 0);
				}
				else {
					writeToAddress16(destinationAddress.raw & (~1), loadFromAddress16(sourceAddress.raw & (~1)));
				}
			}
		 	else {
				if (sourceAddress.mask > EGamePak6 && i == 0) {
					writeToAddress32(destinationAddress.raw & (~3), 0);
				}
				else
				{
					writeToAddress32(destinationAddress.raw & (~3), loadFromAddress32(sourceAddress.raw & (~3)));
				}
			}

			if (DMAControl->destCtrl == DMAcontrol_t::Increment || DMAControl->destCtrl == DMAcontrol_t::IncOrReload)
				destinationAddress = destinationAddress + (DMAControl->transferType == DMAtransfer_t::WORD ? 4 : 2);
			else if (DMAControl->destCtrl == DMAcontrol_t::Decrement)
				destinationAddress = destinationAddress - (DMAControl->transferType == DMAtransfer_t::WORD ? 4 : 2);

			if (DMAControl->sourceCtrl == DMAcontrol_t::Increment)
				sourceAddress = sourceAddress + (DMAControl->transferType == DMAtransfer_t::WORD ? 4 : 2);
			else if (DMAControl->sourceCtrl == DMAcontrol_t::Decrement)
				sourceAddress = sourceAddress - (DMAControl->transferType == DMAtransfer_t::WORD ? 4 : 2);
		}
	}
	if (DMAControl->irq){
		if (InterruptEnableRegister->DMA0 && i == 0)
			InterruptFlagRegister->DMA0 = 1;
		else if (InterruptEnableRegister->DMA1 && i == 1)
			InterruptFlagRegister->DMA1 = 1;
		else if (InterruptEnableRegister->DMA2 && i == 2)
			InterruptFlagRegister->DMA2 = 1;
		else if (InterruptEnableRegister->DMA3 && i == 3)
			InterruptFlagRegister->DMA3 = 1;
	}
}

void startDMA(){
	for (int i = 0; i < 4; i++){
		DMAcontrol* DMAControl = (DMAcontrol*)&IoRAM[ 0xBA + 0xC * i];
		if ((DMAControl->enable) || (InterruptFlagRegister->hBlank && DMAControl->irq == 2) || (InterruptFlagRegister->vBlank && DMAControl->irq == 1)){
			if (DMAControl->timing == DMAtiming_t::VBlank && !InterruptFlagRegister->vBlank) {
				continue;
			}
			if (DMAControl->timing == DMAtiming_t::VBlank && !InterruptFlagRegister->vBlank) {
				continue;
			}

			MemoryAddress sourceAddress = rawLoad32(IoRAM, 0xB0 + 0xC * i);
			MemoryAddress destinationAddress = rawLoad32(IoRAM, 0xB4 + 0xC * i);
			uint32_t wordCount = rawLoad16(IoRAM, 0xB8 + 0xC * i);

			if (sourceAddress.mask < 0x05)
				sourceAddress = sourceAddress & 0x7FFFFFF;
			else
				sourceAddress = sourceAddress &  0xFFFFFFF;

			if (destinationAddress.mask < 0x05)
				destinationAddress = destinationAddress & 0x7FFFFFF;
			else
				destinationAddress = destinationAddress & 0xFFFFFFF;


			if (i == 3 && wordCount == 0)
				wordCount = 0x10000;
			else if (wordCount == 0)
				wordCount = 0x4000;

			if ((i == 1 || i == 2) && DMAControl->timing == DMAtiming_t::Special){
				for (int words = 0; words < 4; words++){
					//writeToAddress32(destinationAddress + 4, loadFromAddress32(sourceAddress)); //fifo mode, do addr control logic sound control. do later
				}
				return;
			}

			if (destinationAddress.mask == 4 || sourceAddress.mask == 4)
			{
				continue;
			}

			if (destinationAddress.mask >= EGamePak1 && destinationAddress.mask <= EGamePak6)
			{
				continue;
			}

			if (i != 3)
			{
				if (sourceAddress.mask >= EGamePak1 && sourceAddress.mask <= EGamePak6)
				{
					continue;
				}
				if (destinationAddress.mask > EGamePak6)
				{
					continue;
				}
			}

			doDMA(DMAControl, i, destinationAddress, sourceAddress, wordCount);
			
			if (!DMAControl->repeat) {
				DMAControl->enable = 0;
			}
			else{
				writeToAddress32(0x40000B4 + 0xC * i, destinationAddress.raw);
			}
		}
	}
}