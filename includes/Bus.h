#ifndef BUS_H
#define BUS_H
#include "Constants.h" 
#include "CPU.h" 


class CPU ;



struct DEVICES {

	CPU* cpu ; 



};


class Bus {
public :
	Bus(){
		for(int i = 0 ; i < bus_size ; i++)
			_bus[i] = 0x00 ; 

	}
	virtual ~Bus(){}

	uint8_t read(uint16_t address) ; 
	void write(uint16_t address , uint8_t value) ; 



private:
	static const std::size_t bus_size = 64 * 1024 ;  
	std::array<uint8_t , bus_size> _bus ; 
	DEVICES devices ; 










};



















#endif
