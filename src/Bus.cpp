#include "../includes/Bus.h"









uint8_t BUS::read(uint16_t address) {
	if(address >= 0x0000 && address <= 0xFFFF)
		return _bus[address] ; 
	return 0 ; 

}



void BUS::write(uint16_t address , uint8_t value){
	if(address >= 0x0000 && address <= 0xFFFF)
		_bus[address] = value ; 


}


void BUS::write_stack(uint8_t sp , uint8_t value){
		_bus[(sp | 0x0100)] = value ; 
}

uint8_t BUS::read_stack(uint8_t sp) {
		return _bus[(sp | 0x0100)] ;

}


void BUS::init_rom(std::vector<uint8_t> buffer){
	for(std::size_t i = 0 ; i < buffer.size() ; i++)
		_bus[PRGROM::BEGIN + i] = buffer[i] ; 

}
