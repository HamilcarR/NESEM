#ifndef CPU_H
#define CPU_H
#include "Constants.h"
#include "Bus.h" 



class BUS ; 
class CPU{

//structs
public:
enum PROCSTATUS  {
	C = (1 << 0), // carry flag 
	Z = (1 << 1) , //zero flag
	I = (1 << 2) , //interrupt disable
	D = (1 << 3) , //Decimal mode
	B = (1 << 4) , //Break command
	U = (1 << 5), //unused 
	V = (1 << 6) , //Overflow flag
	N = (1 << 7)   // Negative flag


};



struct REGISTERS {
	uint16_t PC ; //program counter 
	uint8_t	 SP ; //stack pointer , descending , init to 0xFF
	uint8_t  A ;// accumulator
	uint8_t  X ; // X index register
	uint8_t  Y ; // Y index register
	uint8_t  P ; // processor status

};


struct INSTRUCTION{
	std::string mnemonic ; 
	uint8_t opcode ; 
	uint8_t (CPU::*addressing_mode)(void) ; 
	uint8_t (CPU::*instruction)(void) ; 
	uint8_t instruction_bytes ; 
	uint8_t machine_cycles ; 
	uint8_t additionnal_cycles ; 
};



struct REGSTATES{
	static const uint8_t PROCESSOR_FLAG_POWER_ON = 0x34 ;//IRQ disable flag and Break to true  
	static const uint8_t PROCESSOR_FLAG_RESET = 0x04 ; //IRQ disable true 
	static const uint8_t REGISTER_STACK_POWER_ON = 0xFD ; 




};








public:
	CPU(BUS *b){
		bus = b ;
		ticks = 1 ;
		current_opcode = 0x00 ;
		rel_addr = 0x00 ; 
		abs_addr = 0x0000 ;
		instruction_count = 0x0000;
		fill_table();
	}
	virtual ~CPU(){}
	void update_flag(PROCSTATUS P , bool expr ) ; 
	uint8_t get_flag(PROCSTATUS P) ; 
	uint8_t read(uint16_t address);
	void write(uint16_t address , uint8_t value) ;
	void get() ; //reads opcode from opcode list
	void fill_table(); 
	//initialize the states of registers and memory  after a power up 
	void power_on();
	//initialize the states of registers and memory after a regular reset
	void soft_reset();
	void power_off(); 
	const REGISTERS getRegisters() const {return registers ;} 
	const BUS*      getBus()const {return bus ; }

//addressing modes
	uint8_t ZP(); //zero page
	uint8_t ZPX(); //indexed zero page X
	uint8_t ZPY(); //indexed zero page Y
	uint8_t ABS(); //absolute
	uint8_t ABSX(); //absolute indexed X
	uint8_t ABSY(); //absolute indexed Y
	uint8_t IND();  //indirect 
	uint8_t IMP();  //implied
	uint8_t ACC();  //accumulator
	uint8_t IMM(); //immediate 
	uint8_t REL(); //relative
	uint8_t INDX();//indirect indexed X
	uint8_t INDY() ; //indirect indexed Y

//clock 
	void clock();

//interrupts
	void IRQ() ; //maskable  
	void NMI() ; //non maskable interrupts
	void RESET(); //reset

//address pointers and stored data
	uint16_t rel_addr; 
	uint16_t abs_addr;
	uint8_t data ; 
	unsigned int ticks ; 
	uint8_t current_opcode = 0 ; 
//instructions matrix 
	std::vector<INSTRUCTION> opcodes_table ; 	
//instructions count (debug) 	
	uint16_t instruction_count ; 
private:
	BUS* bus ;
	REGISTERS registers ; 






private:
	void registers_reset() ; 















public:
//instructions : 
uint8_t ADC();// add memory to accumulator with carry
uint8_t AND();// AND memory with accumulator
uint8_t ASL();// shift left one bit memory or accumulator
uint8_t BCC();// branch on carry clear
uint8_t BCS();// branch on carry set
uint8_t BEQ();// branch on result zero 
uint8_t BIT();// test bits in mem with accumulator
uint8_t BMI();// branch on result minus 
uint8_t BNE();// branch on result not zero 
uint8_t BPL();// branch on result plus
uint8_t BRK();// force break 
uint8_t BVC();// branch on overflow clear
uint8_t BVS();// branch on overflow set
uint8_t CLC();// clear carry flag
uint8_t CLD();// clear decimal mode // not used
uint8_t CLI();// clear interrupt disable bit
uint8_t CLV();// clear overflow flag
uint8_t CMP();// compare memory and accumulator
uint8_t CPX();// compare memory and index x 
uint8_t CPY();// compare memory and index y
uint8_t DEC();// decrement memory by one
uint8_t DEX();// decrement index x by one
uint8_t DEY();// decrement index y by one
uint8_t EOR();// "exclusive or" memory with accumulator
uint8_t INC();// increment memory by one
uint8_t INX();// increment index x by one
uint8_t INY();// increment index y by one
uint8_t JMP();// jump to new location
uint8_t JSR();// jump to new location saving return address 
uint8_t LDA();// load accumulator with memory 
uint8_t LDX();// load index x with memory
uint8_t LDY();// load index y with memory
uint8_t LSR();// shift one bit right memory or accumulator
uint8_t NOP();// no operation
uint8_t ORA();// "or" memory with accumulator
uint8_t PHA();// push accumulator on stack
uint8_t PHP();// push proessor status on stack
uint8_t PLA();// pull accumulator from stack
uint8_t PLP();// pull processor status from stack
uint8_t ROL();// rotate one bit left memory or accumulator
uint8_t ROR();// rotate one bit right memory or accumulator
uint8_t RTI();// return from interrupt 
uint8_t RTS();// return from subroutine
uint8_t SBC();// substract memory from accumulator with borrow
uint8_t SEC();// set carry flag
uint8_t SED();// set decimal mode // not used
uint8_t SEI();// set interrupt disable status
uint8_t STA();// store accumulator in memory
uint8_t STX();// store index x in memory
uint8_t STY();// store index y in memory 
uint8_t TAX();// transfer accumulator to index x
uint8_t TAY();// transfer accumulator to index y
uint8_t TSX();// transfer stack pointer to index x
uint8_t TXA();// transfer index x to accumulator
uint8_t TXS();// transfer index x to stack register
uint8_t TYA();// transfer index y to accumulator
uint8_t ILL();//illegal opcode 




};





















#endif 
