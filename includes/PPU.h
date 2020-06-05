#ifndef PPU_H
#define PPU_H

#include "Constants.h" 
#include "Bus.h"
#include "CPU.h"





class PPUBUS ; 
class BUS ; 

class PPU {





public:

enum PPUCTRL {
	NL = (1 << 0) , //Base nametable address Low and High 
	NH = (1 << 1) , // 0 = $2000 , 1 = $2400 , 2 = $2800 , 3 = $2C00 
	I  = (1 << 2) , //Vram address increment per CPU read/write of PPUDATA , 0 = add 1  , 1 = add 32
	S  = (1 << 3) , //Sprite pattern table address for 8x8 sprites
	B  = (1 << 4) , //Background pattern table addr 
	H  = (1 << 5) , //Sprite size
	P  = (1 << 6) , //PPU master/slave select
	V  = (1 << 7)   //NMI generate , Vblank 

};


























	PPU(BUS* cpubus , PPUBUS *ppubus){
		cpu_bus = cpubus ; 
		ppu_bus = ppubus ; 
	}

	virtual ~ PPU(){} 




	uint16_t address ; 
private:
	
	BUS *cpu_bus ; 
	PPUBUS *ppu_bus ; 








};



















#endif
