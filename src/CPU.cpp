#include "../includes/CPU.h"






static void registers_reset(REGISTERS &registers){
	registers.A = 0 ; 
	registers.X = 0 ; 
	registers.Y = 0 ; 
	registers.P = 0x24 ;
	registers.SP = 0xFF ; 





}







void CPU::update_flag(PROCSTATUS P , bool expr){
	if(expr)
		registers.P = registers.P | P ; 
	else
		registers.P = registers.P & ~P ; 


}
bool CPU::get_flag(PROCSTATUS P){
	
	return (registers.P & P) != 0 ; 

}


uint8_t CPU::read(uint16_t address){
	uint16_t content = _bus->read(address) ; 
	return content ; 


}


void CPU::write(uint16_t address , uint8_t value){
	
	_bus->write(address , value) ; 

}






/************************************ BEGIN OF ADDRESSING MODE ********************************/
uint8_t CPU::ZP(){
	uint8_t val = read(registers.PC++) ; 
	abs_addr = 0x00FF & val ; 
	return 0 ; 
}


uint8_t CPU::ZPX(){
	uint8_t val = read(registers.PC++) + registers.X ; 
	abs_addr = 0x00FF & val ; 
	return 0 ; 
}

uint8_t CPU::ZPY(){
	uint8_t val = read(registers.PC++) + registers.Y ; 
	abs_addr = 0x00FF & val ; 
	return 0 ; 
}

uint8_t CPU::ABS(){
	uint8_t low = read(registers.PC++) ; 
	uint8_t high = read(registers.PC++) ; 
	abs_addr = (high << 8) | low ; 
	return 0 ; 
}

uint8_t CPU::ABSX(){
	uint8_t low = read(registers.PC) ; 
	registers.PC ++ ; 
	uint8_t high = read(registers.PC) ; 
	registers.PC++ ; 
	abs_addr = (high << 8) | low ; 
	abs_addr += registers.X ; 
	if((abs_addr & 0xFF00) != ((high << 8) & 0xFF00))
		return 1 ; 
	else
		return 0 ; 

}


uint8_t CPU::ABSY(){
	uint8_t low = read(registers.PC) ; 
	registers.PC++;
	uint8_t high = read(registers.PC);
	registers.PC++;
	abs_addr = ( high << 8) | low ; 
	abs_addr += registers.Y ; 
	if((abs_addr & 0xFF00) != ((high << 8) & 0xFF00))
		return 1 ; 
	else
		return 0 ; 
	
}



uint8_t CPU::IND(){
	uint8_t low = read(registers.PC); 
	registers.PC++;
	uint8_t high = read(registers.PC); 
	registers.PC++;
	uint16_t val= (high << 8) | low ;

	/*jmp bug :
	 * http://nesdev.com/6502bugs.txt */

	if(value == 0x00FF)
		abs_addr = (read(val & 0xFF00) << 8) | read(val) ; 
	else
		abs_addr = (read(val + 1 ) << 8) | read(val) ; 
	return 0; 
}


uint8_t CPU::IMP(){
	value = registers.A ; 
	return 0 ; 
}

uint8_t CPU::ACC(){
	value = registers.A ; 
	return 0 ; 

}


uint8_t CPU::IMM(){
	abs_addr = registers.PC ; 
	registers.PC++ ; 
	return 0 ; 

}


uint8_t CPU::REL(){
	rel_addr = read(registers.PC);
	registers.PC++;
	bool negative = (rel_addr & 0x80) ;
	if(negative)
		rel_addr = rel_addr | 0xFF00 ; // jumping backwards 
	return 0 ; 

}


//in zero page
uint8_t CPU::INDX(){
	uint8_t val = read(registers.PC++) ; 
	uint16_t ptr = 0x00FF & val ;
	uint8_t low = read((ptr + registers.X) & 0x00FF);
	uint8_t high = read((ptr + registers.X + 1 ) & 0x00FF); 
	abs_addr = (high << 8) | low ; 
	return 0 ; 


}

uint8_t CPU::INDY(){
	uint8_t val = read(registers.PC++) ; 
	uint16_t ptr = 0x00FF & val ; 
	uint8_t low = read(ptr & 0x00FF) ; 
	uint8_t high = read((ptr + 1) & 0x00FF ) ; 
	abs_addr = ((high << 8) | low) + registers.Y ; 
	if( (abs_addr & 0xFF00) != (high << 8))
		return 1 ; 
	else
		return 0 ; 



}

/************************************ END OF ADDRESSING MODE ********************************/


/************************************ CLOCK ********************************/
void CPU::clock(){
	if(ticks == 0){


	}
	ticks-- ;

}






/************************************ BEGIN OF INTERRUPTS ********************************/

void CPU::RESET(){
	uint8_t ptr1 = read(0xFFFC) ;
	uint8_t ptr2 = read(0xFFFD) ; 
	uint16_t value = (ptr2 << 8) | ptr1 ; 
	registers_reset(registers); 
	registers.PC = value ; 
	rel_addr = 0 ; 
	abs_addr = 0 ; 
	value = 0 ; 
	ticks = 0 ; 

}






void CPU::IRQ(){





}


void CPU::NMI(){




}



/************************************ END OF INTERRUPT ********************************/














//instructions

uint8_t CPU::ADC() { }
uint8_t CPU::AND() { }
uint8_t CPU::ASL() { }
uint8_t CPU::BCC() { }
uint8_t CPU::BCS() { }
uint8_t CPU::BEQ() { }
uint8_t CPU::BIT() { }
uint8_t CPU::BMI() { }
uint8_t CPU::BNE() { }
uint8_t CPU::BPL() { }
uint8_t CPU::BRK() { }
uint8_t CPU::BVC() { }
uint8_t CPU::BVS() { }
uint8_t CPU::CLC() { }
uint8_t CPU::CLD() { }
uint8_t CPU::CLI() { }
uint8_t CPU::CLV() { }
uint8_t CPU::CMP() { }
uint8_t CPU::CPX() { }
uint8_t CPU::CPY() { }
uint8_t CPU::DEC() { }
uint8_t CPU::DEX() { }
uint8_t CPU::DEY() { }
uint8_t CPU::EOR() { }
uint8_t CPU::INC() { }
uint8_t CPU::INX() { }
uint8_t CPU::INY() { }
uint8_t CPU::JMP() { }
uint8_t CPU::JSR() { }
uint8_t CPU::LDA() { }
uint8_t CPU::LDX() { }
uint8_t CPU::LDY() { }
uint8_t CPU::LSR() { }
uint8_t CPU::NOP() { }
uint8_t CPU::ORA() { }
uint8_t CPU::PHA() { }
uint8_t CPU::PHP() { }
uint8_t CPU::PLA() { }
uint8_t CPU::PLP() { }
uint8_t CPU::ROL() { }
uint8_t CPU::ROR() { }
uint8_t CPU::RTI() { }
uint8_t CPU::RTS() { }
uint8_t CPU::SBC() { }
uint8_t CPU::SEC() { }
uint8_t CPU::SED() { }
uint8_t CPU::SEI() { }
uint8_t CPU::STA() { }
uint8_t CPU::STX() { }
uint8_t CPU::STY() { }
uint8_t CPU::TAX() { }
uint8_t CPU::TAY() { }
uint8_t CPU::TSX() { }
uint8_t CPU::TXA() { }
uint8_t CPU::TXS() { }
uint8_t CPU::TYA() { }
 


