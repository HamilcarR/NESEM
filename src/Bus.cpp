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
		return _bus[address] ; 

}



void BUS::write(uint16_t address , uint8_t value){
		_bus[address] = value ; 
		mirror(address , value);


}


void BUS::write_stack(uint8_t sp , uint8_t value){
		_bus[(sp | STACK::BEGIN)] = value ; 
		mirror((STACK::BEGIN | sp) , value);
}

uint8_t BUS::read_stack(uint8_t sp) {
		return _bus[(sp | 0x0100)] ;

}


void BUS::init_rom(std::vector<uint8_t> buffer , uint16_t address ){
	for(std::size_t i = 0 ; i < buffer.size() ; i++){
		assert((address + i) <= 0xFFFF) ; 
		_bus[address + i] = buffer[i] ; 
	}

}




/****************************************************** PPU BUS ******************************************************/







bool PPUBUS::isMirrored(uint16_t address) const {
	if(address >= NAMETABLES::N_TABLE_0_BEGIN  && address <= 0x2EFF)
		return true ; 
	if(address <= PALETTES::SPRITE_PALETTE_END && address >= PALETTES::IMG_PALETTE_BEGIN)
		return true ; 
	return false ; 

}
/*Locations between $2000 and $2EFF are mirrored in memory $3000-$3EFF 
 *Locations between $3F00 and $3F1F are mirrored in memory $3F20-$3FFF */

void PPUBUS::mirror(uint16_t address , uint8_t value){
	if(address >= NAMETABLES::N_TABLE_0_BEGIN && address <= 0x2EFF)
		_bus[address | MIRRORS::NT_BEGIN] = value ; 
	if(address <= PALETTES::SPRITE_PALETTE_END && address >= PALETTES::IMG_PALETTE_BEGIN) 
		_bus[address | MIRRORS::PT_BEGIN] = value ; 
			

}

uint8_t PPUBUS::read(uint16_t address) const {
	if(address >= 0x0000 && address <= MIRRORS::PT_END)
		return _bus[address] ; 
	else
		return 0; 
}


void PPUBUS::write(uint16_t address , uint8_t value){
	_bus[address] = value ;
	//All the locations between $0000 and $3FFF are mirrored in memory $4000-$FFFF
	_bus[address | MIRRORS::WRAP_BEGIN] = value ; 
	mirror(address , value) ; 
}





