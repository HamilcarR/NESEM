#ifndef BUS_H
#define BUS_H
#include "Constants.h" 
#include "CPU.h" 






class CPU ;



class BUSBASIC {
public:
	virtual uint8_t read(uint16_t address)             const  = 0 ; 
	virtual void    write(uint16_t address , uint8_t value)   = 0 ; 
private:
	virtual void    mirror(uint16_t address , uint8_t value)  = 0 ;
	virtual bool    isMirrored(uint16_t address)       const  = 0 ; 


};










//CPU BUS
class BUS : public BUSBASIC {
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
	static const uint16_t BANK2_BEGIN = 0xC000 ; 


};
//Addresses in range 0x0000 - 0x07FFF are mirrored in the range 0x8000 - 0x1FFF every 0x0800 bytes
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












//PPU registers , mirrored
struct PPUREG{
	static const uint16_t BEGIN = 0x2000 ; 
	static const uint16_t END   = 0x2007 ;

	static const uint16_t PPUCTRL = 0x2000 ; 
	static const uint16_t PPUMASK = 0x2001 ;
	static const uint16_t PPUSTATUS = 0x2002 ; 
	static const uint16_t OAMADDR = 0x2003 ; 
 	static const uint16_t OAMDATA = 0x2004 ;
	static const uint16_t PPUSCROLL = 0x2005 ; 
	static const uint16_t PPUADDR = 0x2006 ; 
	static const uint16_t PPUDATA = 0x2007 ; 
	static const uint16_t OAMDMA = 0x4014 ; //DMA register


	

};

struct PPUREGMIRROR{
	static const uint16_t BEGIN = 0x2008 ; 
	static const uint16_t END   = 0x3FFF ; 
	static const uint8_t PADDING =  0x08 ; 

};


	BUS(){
		for(std::size_t i = 0 ; i < bus_size ; i++){
			_bus[i] = 0x00 ; 
		}
		write(IRVECT::RES_VECTOR_LOW , 0x00) ; 
		write(IRVECT::RES_VECTOR_HIGH , 0x80) ; 


	}
	virtual ~BUS(){}

	uint8_t read(uint16_t address) const  ; // return the value of any memory address within bus range 
	void write(uint16_t address , uint8_t value) ; //write value to any memory address 
	void write_stack(uint8_t sp , uint8_t value); //write only to stack , provide only the stack pointer addr
	uint8_t read_stack(uint8_t sp) ; 	//read the value pointing by the stack pointer 
	void init_rom(std::vector<uint8_t> buffer) ; //rom initialisation 
	uint16_t get_reset_vector(); //reads reset vector values on memory




private:
	static const std::size_t bus_size = 64 * 1024 ;  
	std::array<uint8_t , bus_size> _bus ; 
	DEVICES devices ; 
	STACK _stack ; //descending stack 	




	bool isMirrored(uint16_t address) const ; 
	void mirror(uint16_t address , uint8_t value) ;


};



















//VRAM 
class PPUBUS : public BUSBASIC {
public:
	PPUBUS(){}
	virtual ~PPUBUS(){
		for(std::size_t i = 0 ; i < bus_size ; i++)
			_bus[i] = 0x00 ; 
	}
	uint8_t read(uint16_t address) const ;
	void    write(uint16_t address , uint8_t value); 




private:
	static const std::size_t bus_size = 16 * 1024 ; 
	std::array<uint8_t , bus_size> _bus ; //PPU's own memory


	bool    isMirrored(uint16_t address) const ; 
	void    mirror(uint16_t address , uint8_t value) ; 


	



};





















#endif
