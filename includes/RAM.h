#ifndef RAM_H
#define RAM_H
#include "Bus.h"
#include "Constants.h"




class RAM{

public:
	RAM(Bus* bus){
		_bus = bus ; 
		pointer = start_address ; 

	}


private:
	

	static const uint16_t start_address = 0x0200 ; 
	static const uint16_t end_address = 0x0800 ; 
	Bus* _bus ; 
	uint16_t pointer;






};


















#endif
