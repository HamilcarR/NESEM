#include "../includes/Bus.h"









bool BUS::isMirrored(uint16_t address){
	bool m1 = address <= MIRROR::A_END && address >= MIRROR::A_BEGIN ; 
	bool m2 = address >= MIRROR::B_BEGIN && address <= MIRROR::B_END ; 
	bool m3 = address >= MIRROR::C_BEGIN && address <= MIRROR::C_END ; 
	bool m4 = address >= MIRROR::D_BEGIN && address <= MIRROR::D_END ; 
	return m1 || m2 || m3 || m4 ; 
	
}







void BUS::mirror(uint16_t address , uint8_t value){
	if(isMirrored(address)){
		if((address + MIRROR::PADDING) > MIRROR::D_END) 
			for(uint16_t i = 0 ; i < 4 ; i++) 
				_bus[address - i * MIRROR::PADDING] = value ; 

		else if((address - MIRROR::PADDING) < MIRROR::A_BEGIN)
			for(uint16_t i = 0 ; i < 4 ; i++)
				_bus[address + i * MIRROR::PADDING] = value ; 

		else{

			int16_t i = address ;
			int16_t j = address ; 
			while(i <= MIRROR::D_END){
				_bus[i] = value ; 
				i += MIRROR::PADDING ;

			}

			while(j >= MIRROR::A_BEGIN){
				_bus[j] = value ; 
				j -= MIRROR::PADDING ; 
				
			}

		}


	}



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
	for(std::size_t i = 0 ; i < buffer.size() ; i++)
		_bus[PRGROM::BEGIN + i] = buffer[i] ; 

}
