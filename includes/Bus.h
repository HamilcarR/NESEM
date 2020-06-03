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

struct MIRROR{
	static const uint16_t A_BEGIN = 0x0000 ; 
	static const uint16_t A_END = 0x07FF ; 

	static const uint16_t B_BEGIN = 0x0800 ; 
	static const uint16_t B_END = 0x0FFF ; 

	static const uint16_t C_BEGIN = 0x1000 ; 
	static const uint16_t C_END = 0x17FF ;

	static const uint16_t D_BEGIN = 0x1800 ; 
	static const uint16_t D_END = 0x1FFF ; 
	
	static const uint16_t PADDING = 0x0800 ; 

};
	BUS(){
		for(std::size_t i = 0 ; i < bus_size ; i++){
			_bus[i] = 0x00 ; 
		}
		write(IRVECT::RES_VECTOR_LOW , 0x00) ; 
		write(IRVECT::RES_VECTOR_HIGH , 0x80) ; 


	}
	virtual ~BUS(){}

	uint8_t read(uint16_t address) const  ; 
	void write(uint16_t address , uint8_t value) ; 
	void write_stack(uint8_t sp , uint8_t value); 	
	uint8_t read_stack(uint8_t sp) ; 	
	void init_rom(std::vector<uint8_t> buffer) ; 
private:
	static const std::size_t bus_size = 64 * 1024 ;  
	std::array<uint8_t , bus_size> _bus ; 
	DEVICES devices ; 
	STACK _stack ; //descending stack 	




	bool isMirrored(uint16_t address) ; 
	void mirror(uint16_t address , uint8_t value) ;


};



















#endif
