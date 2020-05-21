#include "../includes/Bus.h"









uint8_t Bus::read(uint16_t address) {
	if(address <= 0x0000 && address >= 0xFFFF)
		return _bus[address] ; 
	return 0 ; 

}



void Bus::write(uint16_t address , uint8_t value){
	if(address <= 0x0000 && address >= 0xFFFF)
		_bus[address] = value ; 


}



