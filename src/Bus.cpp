#include "../includes/Bus.h"









bool BUS::isMirrored(uint16_t address) const{

	bool m1 = address <= MIRROR::A_END && address >= MIRROR::A_BEGIN ; 
	bool m2 = address <= PPUREG::END && address >= PPUREG::BEGIN ; 
	return m1 || m2 ; 	
	
}



uint16_t BUS::get_reset_vector(){
	uint8_t low = read(IRVECT::RES_VECTOR_LOW) ; 
	uint8_t high = read(IRVECT::RES_VECTOR_HIGH) ; 

	return ((high & 0xFFFF) << 8) | low ; 

}



void BUS::mirror(uint16_t address , uint8_t value){
	if(address <= MIRROR::A_END && address >= MIRROR::A_BEGIN ) 
		for(uint16_t i = 0 ; i < 4 ; i++) 
			_bus[address + i * MIRROR::PADDING] = value ;
	if(address <= PPUREG::END && address >= PPUREG::BEGIN )
		for(uint16_t i = 0 ; (address + i) < PPUREGMIRROR::END ; i+=PPUREGMIRROR::PADDING)
			_bus[address + i] = value ; 



}




uint8_t BUS::read(uint16_t address) const  {
	if(address >= 0x0000 && address <= 0xFFFF)
		return _bus[address] ; 
	return 0 ; 

}



void BUS::write(uint16_t address , uint8_t value){

	if(address >= 0x0000 && address <= 0xFFFF){
		_bus[address] = value ; 
		mirror(address , value);
	}


}


void BUS::write_stack(uint8_t sp , uint8_t value){
		_bus[(sp | STACK::BEGIN)] = value ; 
		mirror((STACK::BEGIN | sp) , value);
}

uint8_t BUS::read_stack(uint8_t sp) {
		return _bus[(sp | 0x0100)] ;

}


void BUS::init_rom(std::vector<uint8_t> buffer){
	for(std::size_t i = 0 ; i < buffer.size() ; i++){
		_bus[PRGROM::BEGIN + i] = buffer[i] ; 
		//_bus[PRGROM::BANK2_BEGIN + i] = buffer[i] ; 
	}

}
















