#include "../includes/PPU.h"













void PPU::step_address(){
	address += (registers.PPUCTRL & 0x20) == 1 ? 32 : 1 ; 

}






void PPU::step_address(){
	

}









/*3 PPU cycles for 1 CPU cycle*/
void PPU::clock(){



	ticks++; 

}























