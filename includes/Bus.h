#ifndef BUS_H
#define BUS_H
#include "Constants.h" 
#include "CPU.h" 






class CPU ;



class BUS {
public :

struct IRVECT {
	static const uint16_t IRQ_INTERRUPT_VECTOR_LOW = 0xFFFE ; 
	static const uint16_t IRQ_INTERRUPT_VECTOR_HIGH = 0xFFFF;
	static const uint16_t RES_VECTOR_LOW = 0xFFFC ; //reset
	static const uint16_t RES_VECTOR_HIGH = 0xFFFD ; 
	static const uint16_t NMI_INTERRUPT_VECTOR_LOW = 0xFFFA ; 
	static const uint16_t NMI_INTERRUPT_VECTOR_HIGH = 0xFFFB ; 
};

struct DEVICES {
	CPU* cpu ; 
};

struct STACK {
	static const uint16_t BEGIN = 0x0100 ; 
	static const uint16_t END = 0x01FF ; 

};
struct PRGROM{
	static const uint16_t BEGIN = 0x8000 ; 
	static const uint16_t END = 0xFFFF ; 

};


	BUS(){
		for(std::size_t i = 0 ; i < bus_size ; i++){
			_bus[i] = 0x00 ; 
		}
		write(IRVECT::RES_VECTOR_LOW , 0x00) ; 
		write(IRVECT::RES_VECTOR_HIGH , 0x80) ; 


	}
	virtual ~BUS(){}

	uint8_t read(uint16_t address) ; 
	void write(uint16_t address , uint8_t value) ; 
	void write_stack(uint8_t sp , uint8_t value); 	
	uint8_t read_stack(uint8_t sp) ; 	
	void init_rom(std::vector<uint8_t> buffer) ; 
private:
	static const std::size_t bus_size = 64 * 1024 ;  
	std::array<uint8_t , bus_size> _bus ; 
	DEVICES devices ; 
	STACK _stack ; //descending stack 	








};



















#endif
