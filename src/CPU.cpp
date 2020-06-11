#include "../includes/CPU.h"





void CPU::registers_reset(){
	registers.A = 0 ; 
	registers.X = 0 ; 
	registers.Y = 0 ; 
	registers.P = 0x24 ;
	registers.SP = BUS::STACK::END & 0xFD; // put to FF or FD ? TODO  
	//registers.PC = bus->get_reset_vector(); 
	registers.PC = 0xC000 ; 
}





/* https://wiki.nesdev.com/w/index.php/CPU_power_up_state */
void CPU::power_on(){
	registers_reset() ;
	//soft_reset();  



}


void CPU::soft_reset(){
	RESET() ; 

}

void CPU::power_off(){



}

void CPU::update_flag(PROCSTATUS P , bool expr){
	if(expr)
		registers.P |=  P ; 
	else
		registers.P &=  ~P ; 


}
uint8_t CPU::get_flag(PROCSTATUS P){
	
	return (registers.P & P) != 0 ? 0x01 : 0x00 ; 

}


uint8_t CPU::read(uint16_t address){
	uint16_t content = bus->read(address) ; 
	return content ; 


}


void CPU::write(uint16_t address , uint8_t value){
	
	bus->write(address , value) ; 

}


void CPU::get(){
	INSTRUCTION instruct = opcodes_table[current_opcode] ; 
	if(instruct.addressing_mode != &CPU::IMP)
		data = read(abs_addr); 

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
	abs_addr = ((high & 0x00FF) << 8) | low ; 
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

	if(val == 0x00FF)
		abs_addr = (read(val & 0xFF00) << 8) | read(val) ; 
	else
		abs_addr = (read(val + 1 ) << 8) | read(val) ; 
	return 0; 
}


uint8_t CPU::IMP(){
	data = registers.A ; 
	return 0 ; 
}

uint8_t CPU::ACC(){
	data = registers.A ; 
	return 0 ; 

}


uint8_t CPU::IMM(){
	abs_addr = registers.PC ; 
	registers.PC++ ; 
	return 0 ; 

}


uint8_t CPU::REL(){
	rel_addr = read(registers.PC++);
	bool negative = (rel_addr & 0x80) ;
	if(negative)
		rel_addr |= 0xFF00 ; // jumping backwards using wrap around 
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
		uint8_t opcode = read(registers.PC++) ;
		current_opcode = opcode ; 
		INSTRUCTION instruction = opcodes_table[opcode] ;
		uint8_t cycles = instruction.machine_cycles ; 
		uint8_t addressing_cycles = (this->*instruction.addressing_mode)(); 		
		uint8_t instruction_cycles = (this->*instruction.instruction)(); 
		ticks+= cycles + (addressing_cycles & instruction_cycles) ;	
#ifdef DEBUG
		std::ofstream out("emu.log" , std::ios::app | std::ios::out | std::ios::ate); 
		out << instruction_count + 1 << "   " << instruction.mnemonic << "  " << UTILITY::format(instruction.opcode);
		out << "   		A:" <<UTILITY::format(registers.A) << " ; X:" << UTILITY::format(registers.X) << " ; Y:" ; 
		out << UTILITY::format(registers.Y) << " ; P:" << UTILITY::format(registers.P) << " ; SP:" << UTILITY::format(registers.SP) << std::endl; 
		out.close();
#endif
		instruction_count ++ ; 
	}
	ticks-- ;

}






/************************************ BEGIN OF INTERRUPTS ********************************/

void CPU::RESET(){
	uint8_t ptr1 = read(BUS::IRVECT::RES_VECTOR_LOW) ;
	uint8_t ptr2 = read(BUS::IRVECT::RES_VECTOR_HIGH) ; 
	uint16_t value = (ptr2 << 8) | ptr1 ; 
	update_flag(I , true) ; 
	registers.PC = value ; 

}

void CPU::IRQ(){
	uint8_t msb = (registers.PC >> 8) & 0xFF ; 
	uint8_t lsb = registers.PC & 0xFF ;
	registers.SP-- ; 
	bus->write_stack(registers.SP-- , msb) ; 
	bus->write_stack(registers.SP-- , lsb) ;
	bus->write_stack(registers.SP , registers.P) ; 
	update_flag(I , true) ;
	uint8_t lsbPC = bus->read(BUS::IRVECT::IRQ_INTERRUPT_VECTOR_LOW) ; 
	uint8_t msbPC = bus->read(BUS::IRVECT::IRQ_INTERRUPT_VECTOR_HIGH) ; 
	registers.PC = ((msbPC & 0x00FF) << 8) | lsbPC ; 

}

void CPU::NMI(){
	uint8_t msb = (registers.PC >> 8) & 0xFF ; 
	uint8_t lsb = registers.PC & 0xFF ; 
	registers.SP-- ; 
	bus->write_stack(registers.SP-- , msb) ; 
	bus->write_stack(registers.SP-- , lsb) ;
	bus->write_stack(registers.SP , registers.P) ; 
	update_flag(I , true) ;
	uint8_t lsbPC = bus->read(BUS::IRVECT::NMI_INTERRUPT_VECTOR_LOW) ; 
	uint8_t msbPC = bus->read(BUS::IRVECT::NMI_INTERRUPT_VECTOR_HIGH) ; 
	registers.PC = ((msbPC & 0x00FF) << 8) | lsbPC ; 

}



/************************************ END OF INTERRUPT ********************************/














//instructions

uint8_t CPU::ADC() {
	get(); 
	uint8_t datasign = data & 0x80 ; 
	uint8_t carryin = get_flag(C) ; 	
	uint8_t accsign = ( registers.A ) & 0x80 ; 
	uint16_t sum = data + registers.A + carryin;
	uint8_t carryout = (sum & 0x0100)  > 0 ? 1 : 0 ;
	registers.A = data + registers.A + carryin ; 
	update_flag(C , carryout); 
	update_flag(Z , registers.A == 0) ; 
/*check if overflowing ie positive + positive = negative or negative + negative = positive*/
	bool overflow = (datasign == accsign) && ((sum & 0x80) != accsign) ; 
	update_flag(V , overflow); 
	update_flag(N , registers.A & 0x80) ; 
	return 1 ; 


}

uint8_t CPU::AND() {
	get() ; 
	registers.A &= data ;
	update_flag(N , registers.A & 0x80) ; 
	update_flag(Z , registers.A == 0x00) ; 

	return 1 ; 

}

uint8_t CPU::ASL() { 
	get();
	if(opcodes_table[current_opcode].addressing_mode == &CPU::ACC){
		uint8_t carry = registers.A & 0x80 ; 
		update_flag(C , carry != 0) ;
		registers.A = registers.A << 1 ; 	
		update_flag(N , registers.A & 0x80) ;	
		update_flag(Z , registers.A == 0) ; 
	}
	else{
		uint8_t carry = data & 0x80 ; 
		update_flag(C , carry != 0) ;
		data = data << 1 ; 
		update_flag(N , data & 0x80) ;
		update_flag(Z , data == 0) ; 
		write(abs_addr , data) ; 


	}
	return 0 ; 

}

uint8_t CPU::BCC() { 
	get();
	uint8_t carry = get_flag(C); 
	uint16_t oldaddr = abs_addr ; 
	if(carry == 0x00){
		ticks++ ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++ ;
	}
	return 0 ; 


}

uint8_t CPU::BCS() { 
	get(); 
	uint8_t carry = get_flag(C); 
	uint16_t oldaddr = abs_addr ; 
	if(carry){
		ticks++;
		registers.PC += rel_addr;
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++;
	}
	return 0 ; 

}

uint8_t CPU::BEQ() {
	get();
	uint8_t zero = get_flag(Z) ; 
	if(zero){
		ticks++ ; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++; 

	}
	return 0 ; 
	
}

uint8_t CPU::BIT() {
	get();
	uint8_t t = data & registers.A ; 
	update_flag(Z , t == 0); 
	update_flag(V , data & 0x40); 
	update_flag(N , data & 0x80);
	return 0 ; 
}

uint8_t CPU::BMI() {
	get();
	uint8_t negative = get_flag(N); 
	if(negative){
		ticks++ ; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++; 

	}
	return 0 ; 
}

uint8_t CPU::BNE() { 
	get();
	uint8_t zero = get_flag(Z); 
	if(zero == 0x00){
		ticks++ ; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++; 

	}
	return 0 ; 

}

uint8_t CPU::BPL() {
	get();
	uint8_t negative = get_flag(N); 
	if(!negative ){
		ticks++ ; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++; 

	}
	return 0 ; 

}
	
uint8_t CPU::BRK() {
	get() ; 
	registers.SP-- ; 
	bus->write_stack(registers.SP-- , registers.PC & 0xFF) ; 
	bus->write_stack(registers.SP-- , (registers.PC >> 8) & 0xFF) ; 
	bus->write_stack(registers.SP , registers.P) ;
	uint8_t ivectorhi = bus->read(BUS::IRVECT::IRQ_INTERRUPT_VECTOR_HIGH) ; 
	uint8_t ivectorlo = bus->read(BUS::IRVECT::IRQ_INTERRUPT_VECTOR_LOW) ;
	uint16_t ivec =( (ivectorhi & 0x00FF) << 8) | ivectorlo ; 
	registers.PC =  ivec ;
	update_flag(B , true) ;
	return 0 ; 
}


uint8_t CPU::BVC() {
	get(); 
	uint8_t overflow = get_flag(V) ; 
	if(overflow == 0x00) {
		ticks++; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++ ; 

	}
	return 0 ; 
}

uint8_t CPU::BVS() {
	get(); 
	uint8_t overflow = get_flag(V) ; 
	if(overflow) {
		ticks++; 
		uint16_t oldaddr = abs_addr ; 
		registers.PC += rel_addr ; 
		abs_addr = registers.PC ; 
		if((oldaddr & 0xFF00) != (registers.PC & 0xFF00))
			ticks++ ; 

	}
	return 0 ; 
}

uint8_t CPU::CLC() {
	update_flag(C , false) ; 
	return 0 ; 

}

uint8_t CPU::CLD() { 
	update_flag(D , false) ; 
	return 0 ; 
}

uint8_t CPU::CLI() { 
	update_flag(I , false) ; 
	return 0 ; 

}

uint8_t CPU::CLV() {
	update_flag(V , false) ; 
	return 0 ; 
}

uint8_t CPU::CMP() {
	get(); 
	uint8_t result = registers.A - data ; 
	update_flag(C , registers.A >= data) ; 
	update_flag(Z , registers.A == data) ; 
	update_flag(N , result & 0x80) ; 
	return 1 ; 
}

uint8_t CPU::CPX() {
	get(); 
	uint8_t result = registers.X - data ; 
	update_flag(C , registers.X >= data) ; 
	update_flag(Z , registers.X == data) ; 
	update_flag(N , result & 0x80) ; 
	return 0 ; 

}

uint8_t CPU::CPY() {
	get();
	uint8_t result = registers.Y - data ; 
	update_flag(C , registers.Y >= data) ; 
	update_flag(Z , registers.Y == data) ; 
	update_flag(N , result & 0x80) ; 
	return 0 ; 

}

uint8_t CPU::DEC() {
	get() ; 
	uint8_t val = data - 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	write(abs_addr , val) ; 
	return 0 ; 
}

uint8_t CPU::DEX() {
	get() ; 
	uint8_t val = registers.X - 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	registers.X--; 
	return 0 ; 

}

uint8_t CPU::DEY() {
	get() ; 
	uint8_t val = registers.Y - 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	registers.Y--; 
	return 0 ; 

}

uint8_t CPU::EOR() {
	get();  
	uint8_t XOR = registers.A ^ data ;
	registers.A = XOR ; 
	update_flag(Z , XOR == 0x00);
	update_flag(N , XOR & 0x80) ; 
	return 1 ; 

}

uint8_t CPU::INC() {
	get() ; 
	uint8_t val = data + 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	write(abs_addr , val) ; 
	return 0 ; 

}

uint8_t CPU::INX() {
	get() ; 
	uint8_t val = registers.X + 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	registers.X ++ ; 
	return 0 ; 

}

uint8_t CPU::INY() {
	get() ; 
	uint8_t val = registers.Y + 1 ; 
	update_flag(Z , val == 0x00) ; 
	update_flag(N , val & 0x80) ; 
	registers.Y ++ ; 
	return 0 ; 

}

uint8_t CPU::JMP() {
	get() ; 
	registers.PC = abs_addr ; 
	return 0 ; 
}

uint8_t CPU::JSR() {
	get() ; 
	uint16_t rp = registers.PC - 1 ; 
	uint8_t low = rp & 0x00FF ; 
	uint8_t high = (rp & 0xFF00) >> 8 ; 
	bus->write_stack(registers.SP-- , low) ; 
	bus->write_stack(registers.SP-- , high) ; 
	registers.PC = abs_addr ; 
	return 0 ; 
}

uint8_t CPU::LDA() {
	get();
	registers.A = data ; 
	update_flag(Z , registers.A == 0x00) ; 
	update_flag(N , registers.A & 0x80) ; 
	return 1 ; 
}

uint8_t CPU::LDX() {
	get();
	uint8_t t = data ; 
	registers.X = t ; 
	update_flag(Z , registers.X == 0x00) ; 
	update_flag(N , registers.X & 0x80) ; 
	return 1 ; 

}

uint8_t CPU::LDY() { 
	get();
	uint8_t t = data ; 
	registers.Y = t ; 
	update_flag(Z , registers.Y == 0x00) ; 
	update_flag(N , registers.Y & 0x80) ;
	return 1 ; 

}


uint8_t CPU::LSR() {
	get();
	if(opcodes_table[current_opcode].addressing_mode == &CPU::ACC){
		uint8_t carry = registers.A & 0x01 ; 
		update_flag(C , carry != 0) ; 
		registers.A = registers.A >> 1 ; 
		update_flag(Z , registers.A == 0x00) ; 
		update_flag(N , registers.A & 0x80) ; 


	}
	else{
		uint8_t carry = data & 0x01 ; 
		update_flag(C , carry != 0) ; 
		uint8_t result = data >> 1 ; 
		update_flag(Z , result == 0x00) ; 
		update_flag(N , result == 0x80) ; 
		write(abs_addr , result) ;

	}
	return 0 ; 
}

uint8_t CPU::NOP() {
	get();
	return 0 ; 
}

uint8_t CPU::ORA() {
	get() ; 
	registers.A |= data ; 
	update_flag(Z , registers.A == 0x00) ; 
	update_flag(N , registers.A & 0x80) ; 
	return 1 ; 

}

uint8_t CPU::PHA() {
	bus->write_stack(registers.SP-- , registers.A) ; 
	return 0 ; 
}

uint8_t CPU::PHP() {
	bus->write_stack(registers.SP-- , registers.P) ; 
	return 0 ; 
}

uint8_t CPU::PLA() {
	registers.SP++ ; 
	registers.A = bus->read_stack(registers.SP); 
	update_flag(Z , registers.A == 0x00);
	update_flag(N , registers.A & 0x80); 
	return 0 ; 
}

uint8_t CPU::PLP() {
	registers.SP++;
	registers.P = bus->read_stack(registers.SP);
	return 0 ; 
}

uint8_t CPU::ROL() {
	get();
	uint8_t old_carry = get_flag(C) ; 	 
	if(opcodes_table[current_opcode].addressing_mode == &CPU::ACC){
		uint8_t new_carry = registers.A & 0x80 ; 
		update_flag(C , new_carry != 0x00) ; 
		registers.A = (registers.A << 1) | old_carry ; 
		update_flag(Z , registers.A == 0x00); 
		update_flag(N , registers.A & 0x80); 
	}
	else{
		uint8_t new_carry = data & 0x80 ; 
		update_flag(C , new_carry != 0x00) ; 
		uint8_t new_data = (data << 1) | old_carry ; 
		update_flag(Z , new_data == 0x00); 
		update_flag(N , new_data & 0x80) ; 
	}
	return 0 ; 
}


uint8_t CPU::ROR() {
	get();
	uint8_t old_carry = (get_flag(C) << 7) ;
	if(opcodes_table[current_opcode].addressing_mode == &CPU::ACC){
		uint8_t new_carry = registers.A & 0x01 ; 
		update_flag(C , new_carry != 0x00); 
		registers.A = (registers.A >> 1) | old_carry ; 
		update_flag(Z , registers.A == 0x00) ; 
		update_flag(N , registers.A & 0x80) ; 
	}
	else{
		uint8_t new_carry = data & 0x01 ; 
		update_flag(C , new_carry != 0x00) ; 
		uint8_t new_data = (data >> 1) | old_carry ; 
		update_flag(Z , new_data == 0x00) ; 
		update_flag(N , new_data & 0x80) ; 
	}
	return 0 ; 
}

uint8_t CPU::RTI() { 
	get();
	registers.SP++ ; 
	uint8_t flag = bus->read_stack(registers.SP++) ;
	uint8_t PCH = bus->read_stack(registers.SP++) ; 
	uint8_t PCL =  bus->read_stack(registers.SP) ; 
	registers.P = flag ; 
	uint16_t PC = PCH ;
	PC = (PC << 8) | PCL ; 
	registers.PC = PC ; 
	return 0 ; 

	
}

uint8_t CPU::RTS() {
	get();
	registers.SP++ ; 
	uint8_t PCH = bus->read_stack(registers.SP++); 
	uint8_t PCL = bus->read_stack(registers.SP); 
	uint16_t PC = (PCH & 0x00FF) << 8 | PCL ; 
	registers.PC = PC + 1  ; 
	return 0 ; 
}


uint8_t CPU::SBC() {
	get() ; 
	uint8_t datasign = data & 0x80 ; 
	uint8_t carryin  = get_flag(C) ; 	
	uint8_t accsign = (registers.A) & 0x80 ; 
	uint8_t sub =  registers.A - data - (1 - carryin);
	registers.A = sub ; 
/*check if overflowing ie -X -Y > 0 or +X +Y < 0 , with (X , Y) >= 0 */
	bool overflow = (datasign && !accsign && (sub & 0x80)) || (!datasign && accsign && !(sub & 0x80)) ; 	
	update_flag(Z , registers.A == 0) ; 
	update_flag(V , overflow);	
	update_flag(N , registers.A & 0x80) ; 
	update_flag(C , !(registers.A & 0x80)); 

	return 1 ; 


} 

uint8_t CPU::SEC() { 
	update_flag(C , true) ; 
	return 0 ; 

}

uint8_t CPU::SED() {
	update_flag(D , true) ; // not used ?  
	return 0; 
}

uint8_t CPU::SEI() {
	update_flag(I , true); 
	return 0 ; 

}

uint8_t CPU::STA() {
	get(); 
	write(abs_addr , registers.A); 
	return 0 ; 

}

uint8_t CPU::STX() {
	get();
	write(abs_addr , registers.X); 
	return 0 ;
}

uint8_t CPU::STY() {
	get();
	write(abs_addr , registers.Y); 
	return 0 ; 
}

uint8_t CPU::TAX() {
	registers.X = registers.A ; 
	update_flag(N , registers.X & 0x80); 
	update_flag(Z , registers.X == 0x00) ; 
	return 0 ; 
}

uint8_t CPU::TAY() {
	registers.Y = registers.A ; 
	update_flag(N , registers.Y & 0x80); 
	update_flag(Z , registers.Y == 0x00) ; 
	return 0 ; 


}

uint8_t CPU::TSX() {
	registers.X = registers.SP ; 
	update_flag(N , registers.X & 0x80); 
	update_flag(Z , registers.X == 0x00) ; 
	return 0 ; 

}

uint8_t CPU::TXA() {
	registers.A = registers.X ; 
	update_flag(N , registers.A & 0x80); 
	update_flag(Z , registers.A == 0x00) ; 
	return 0 ; 


}

uint8_t CPU::TXS() {
	registers.SP = registers.X ; 
	return 0 ; 

}

uint8_t CPU::TYA() {
	registers.A = registers.Y ; 
	update_flag(N , registers.A & 0x80); 
	update_flag(Z , registers.A == 0x00) ; 
	return 0 ; 

}



uint8_t CPU::ILL(){

	return 0 ; 
}

















/************************************ POPULATING OPCODE TABLE ********************************/
void CPU::fill_table(){
	std::vector<INSTRUCTION> table{

/*
 *	{"Mnemonic" , opcode , address mode , instruction , byte count , machine cycles count , additionnal cycles} 
 */

/*************************     0x00     *************************/	
	
		{"BRK" ,0x00 , &CPU::IMP, &CPU::BRK , 1 , 7 , 0} ,   	 
		{"ORA" ,0x01 , &CPU::INDX, &CPU::ORA , 2 , 6 , 0} , 
		{"???" ,0x02 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,  
		{"???" ,0x03 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} , 
		{"???" ,0x04 , &CPU::ZP , &CPU::ILL , 2 , 3 , 0} ,
		{"ORA" ,0x05 , &CPU::ZP , &CPU::ORA , 2 , 3 , 0} , 
		{"ASL" ,0x06 , &CPU::ZP , &CPU::ASL , 2 , 5 , 0} , 
		{"???" ,0x07 , &CPU::ZP , &CPU::ILL , 2 , 5 , 0} , 
		{"PHP" ,0x08 , &CPU::IMP , &CPU::PHP , 1 , 3 , 0} , 
		{"ORA" ,0x09 , &CPU::IMM , &CPU::ORA , 2 , 2 , 0} ,
		{"ASL" ,0x0A , &CPU::ACC , &CPU::ASL , 1 , 2 , 0} , 
		{"???" ,0x0B , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"???" ,0x0C , &CPU::ABS , &CPU::ILL , 3 , 4 , 0} ,
		{"ORA" ,0x0D , &CPU::ABS , &CPU::ORA , 3 , 4 , 0} , 
		{"ASL" ,0x0E , &CPU::ABS , &CPU::ASL , 3 , 6 , 0} , 
		{"???" ,0x0F , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} ,

/*************************     0x10     *************************/	
		
		{"BPL" ,0x10 , &CPU::REL , &CPU::BPL , 2 , 4 , 3} ,
		{"ORA" ,0x11 , &CPU::INDY , &CPU::ORA , 2 , 6 , 1} ,
		{"???" ,0x12 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x13 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x14 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} ,
		{"ORA" ,0x15 , &CPU::ZPX , &CPU::ORA , 2 , 4 , 0} ,
		{"ASL" ,0x16 , &CPU::ZPX , &CPU::ASL , 2 , 6 , 0} ,
		{"???" ,0x17 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} ,
		{"CLC" ,0x18 , &CPU::IMP , &CPU::CLC , 1 , 2 , 0} ,
		{"ORA" ,0x19 , &CPU::ABSY , &CPU::ORA , 3 , 5 , 1} ,
		{"???" ,0x1A , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0x1B , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0},
		{"???" ,0x1C , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} ,
		{"ORA" ,0x1D , &CPU::ABSX , &CPU::ORA , 3 , 5 , 1} ,
		{"ASL" ,0x1E , &CPU::ABSX , &CPU::ASL , 3 , 7 , 0} , 
		{"???" ,0x1F , &CPU::ABSX , &CPU::ILL , 3 , 7 , 0} ,
		
/*************************     0x20     *************************/

		{"JSR" ,0x20 , &CPU::ABS , &CPU::JSR , 3 , 6 , 0} ,
		{"AND" ,0x21 , &CPU::INDX , &CPU::AND , 2 , 6 , 0} ,
		{"???" ,0x22 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x23 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} ,
		{"BIT" ,0x24 , &CPU::ZP , &CPU::BIT , 2 , 3 , 0} ,
		{"AND" ,0x25 , &CPU::ZP , &CPU::AND , 2 , 3 , 0} ,
		{"ROL" ,0x26 , &CPU::ZP , &CPU::ROL , 2 , 5 , 0} ,
		{"???" ,0x27 , &CPU::ZP , &CPU::ILL , 2 , 5 , 0} ,
		{"PLP" ,0x28 , &CPU::IMP , &CPU::PLP , 1 , 4 , 0} ,
		{"AND" ,0x29 , &CPU::IMM , &CPU::AND , 2 , 2 , 0} , 
		{"ROL" ,0x2A , &CPU::ACC , &CPU::ROL , 1 , 2 , 0} ,
		{"???" ,0x2B , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} ,
		{"BIT" ,0x2C , &CPU::ABS , &CPU::BIT , 3 , 4 , 0} ,
		{"AND" ,0x2D , &CPU::ABS , &CPU::AND , 3 , 4 , 0} ,
		{"ROL" ,0x2E , &CPU::ABS , &CPU::ROL , 3 , 6 , 0} , 
		{"???" ,0x2F , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} ,
		
/*************************     0x30     *************************/	

		{"BMI" ,0x30 , &CPU::REL , &CPU::BMI , 2 , 2 , 3} ,
		{"AND" ,0x31 , &CPU::INDY , &CPU::AND ,2 , 5 , 1} ,
		{"???" ,0x32 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x33 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x34 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} ,
		{"AND" ,0x35 , &CPU::ZPX , &CPU::AND , 2 , 4 , 0} ,
		{"ROL" ,0x36 , &CPU::ZPX , &CPU::ROL , 2 , 6 , 0} ,
		{"???" ,0x37 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} ,
		{"SEC" ,0x38 , &CPU::IMP , &CPU::SEC , 1 , 2 , 0} ,
		{"AND" ,0x39 , &CPU::ABSY , &CPU::AND , 3 , 4 , 1} ,
		{"???" ,0x3A , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x3B , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0} ,
		{"???" ,0x3C , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} ,
		{"AND" ,0x3D , &CPU::ABSX , &CPU::AND , 3 , 4 , 1} ,
		{"ROL" ,0x3E , &CPU::ABSX , &CPU::ROL , 3 , 7 , 0} ,
		{"???" ,0x3F , &CPU::ABSX , &CPU::ILL , 3 , 7 , 0} ,

/*************************     0x40     *************************/	
		
		{"RTI" ,0x40 , &CPU::IMP , &CPU::RTI , 1 , 6 , 0} , 
		{"EOR" ,0x41 , &CPU::INDX , &CPU::EOR , 2 , 6 , 0} ,
		{"???" ,0x42 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x43 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x44 , &CPU::ZP , &CPU::ILL , 2 , 3 , 0} ,
		{"EOR" ,0x45 , &CPU::ZP , &CPU::EOR , 2 , 3 , 0} ,
		{"LSR" ,0x46 , &CPU::ZP , &CPU::LSR , 2 , 5 , 0} ,
		{"???" ,0x47 , &CPU::ZP , &CPU::ILL , 2 , 6 , 0} ,
		{"PHA" ,0x48 , &CPU::IMP , &CPU::PHA , 1 , 3 , 0} ,
		{"EOR" ,0x49 , &CPU::IMM , &CPU::EOR , 2 , 2 , 0} ,
		{"LSR" ,0x4A , &CPU::ACC , &CPU::LSR , 1 , 2 , 0} ,
		{"???" ,0x4B , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} ,
		{"JMP" ,0x4C , &CPU::ABS , &CPU::JMP , 3 , 3 , 0} , 
		{"EOR" ,0x4D , &CPU::ABS , &CPU::EOR , 3 , 4 , 0} ,
		{"LSR" ,0x4E , &CPU::ABS , &CPU::LSR , 3 , 6 , 0} ,
		{"???" ,0x4F , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} ,
		
/*************************     0x50     *************************/	
		
		{"BVC" ,0x50 , &CPU::REL , &CPU::BVC , 2 , 2 , 3} ,
		{"EOR" ,0x51 , &CPU::INDY , &CPU::EOR , 2 , 5 , 1} ,
		{"???" ,0x52 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x53 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x54 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} ,
		{"EOR" ,0x55 , &CPU::ZPX , &CPU::EOR , 2 , 4 , 0} ,
		{"LSR" ,0x56 , &CPU::ZPX , &CPU::LSR , 2 , 6 , 0} ,
		{"???" ,0x57 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} ,
		{"CLI" ,0x58 , &CPU::IMP , &CPU::CLI , 1 , 2 , 0} ,
		{"EOR" ,0x59 , &CPU::ABSY , &CPU::EOR , 3 , 4 , 1} , 
		{"???" ,0x5A , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x5B , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0} ,
		{"???" ,0x5C , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} ,
		{"EOR" ,0x5D , &CPU::ABSX , &CPU::EOR , 3 , 4 , 1} ,
		{"LSR" ,0x5E , &CPU::ABSX , &CPU::LSR , 3 , 7 , 0} ,
		{"???" ,0x5F , &CPU::ABSX , &CPU::ILL , 3 , 7 , 0} ,

/*************************     0x60     *************************/	

		{"RTS" ,0x60 , &CPU::IMP , &CPU::RTS , 1 , 6 , 0} ,
		{"ADC" ,0x61 , &CPU::INDX , &CPU::ADC , 2 , 6 , 0} ,
		{"???" ,0x62 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x63 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x64 , &CPU::ZP , &CPU::ILL , 2 , 3 , 0} , 
		{"ADC" ,0x65 , &CPU::ZP , &CPU::ADC , 2 , 3 , 0} ,
		{"ROR" ,0x66 , &CPU::ZP , &CPU::ROR , 2 , 5 , 0} ,
		{"???" ,0x67 , &CPU::ZP , &CPU::ILL , 2 , 5 , 0} ,
		{"PLA" ,0x68 , &CPU::IMP , &CPU::PLA , 1 , 4 , 0} ,
		{"ADC" ,0x69 , &CPU::IMM , &CPU::ADC , 2 , 2 , 0} , 
		{"ROR" ,0x6A , &CPU::ACC , &CPU::ROR , 1 , 2 , 0} , 
		{"???" ,0x6B , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} ,
		{"JMP" ,0x6C , &CPU::IND , &CPU::JMP , 3 , 5 , 0} , 
		{"ADC" ,0x6D , &CPU::ABS , &CPU::ADC , 3 , 4 , 0} ,
		{"ROR" ,0x6E , &CPU::ABS , &CPU::ROR , 3 , 6 , 0} ,
		{"???" ,0x6F , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} ,

/*************************     0x70     *************************/	

		{"BVS" ,0x70 , &CPU::REL , &CPU::BVS , 2 , 2 , 3} ,
		{"ADC" ,0x71 , &CPU::INDY , &CPU::ADC , 2 , 5 , 1} ,
		{"???" ,0x72 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} ,
		{"???" ,0x73 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} ,
		{"???" ,0x74 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} ,
		{"ADC" ,0x75 , &CPU::ZPX , &CPU::ADC , 2 , 4 , 0} ,
		{"ROR" ,0x76 , &CPU::ZPX , &CPU::ROR , 2 , 6 , 0} ,
		{"???" ,0x77 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} ,
		{"SEI" ,0x78 , &CPU::IMP , &CPU::SEI , 1 , 2 , 0} ,
		{"ADC" ,0x79 , &CPU::ABSY , &CPU::ADC , 3 , 4 , 1} ,
		{"???" ,0x7A , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0x7B , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0} ,
		{"???" ,0x7C , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} , 
		{"ADC" ,0x7D , &CPU::ABSX , &CPU::ADC , 3 , 4 , 1} ,
		{"ROR" ,0x7E , &CPU::ABSX , &CPU::ROR , 3 , 7 , 0} ,
		{"???" ,0x7F , &CPU::ABSX , &CPU::ILL , 3 , 7 , 0} ,

/*************************     0x80     *************************/	

		{"???" ,0x80 , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} ,
		{"STA" ,0x81 , &CPU::INDX , &CPU::STA , 2 , 6 , 0} ,
		{"???" ,0x82 , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"???" ,0x83 , &CPU::INDX , &CPU::ILL , 2 , 6 , 0} ,
		{"STY" ,0x84 , &CPU::ZP , &CPU::STY , 2 , 3 , 0} ,
		{"STA" ,0x85 , &CPU::ZP , &CPU::STA , 2 , 3 , 0} ,
		{"STX" ,0x86 , &CPU::ZP , &CPU::STX , 2 , 3 , 0} ,
		{"???" ,0x87 , &CPU::ZP , &CPU::ILL , 2 , 3 , 0} ,
		{"DEY" ,0x88 , &CPU::IMP , &CPU::DEY , 1 , 2 , 0} , 
		{"???" ,0x89 , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"TXA" ,0x8A , &CPU::IMP , &CPU::TXA , 1 , 2 , 0} ,
		{"???" ,0x8B , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"STY" ,0x8C , &CPU::ABS , &CPU::STY , 3 , 4 , 0} ,
		{"STA" ,0x8D , &CPU::ABS , &CPU::STA , 3 , 4 , 0} ,
		{"STX" ,0x8E , &CPU::ABS , &CPU::STX , 3 , 4 , 0} , 
		{"???" ,0x8F , &CPU::ABS , &CPU::ILL , 3 , 4 , 0} ,

/*************************     0x90     *************************/	

		{"BCC" ,0x90 , &CPU::REL , &CPU::BCC , 2 , 2 , 3} , 
		{"STA" ,0x91 , &CPU::INDY , &CPU::STA , 2 , 6 , 0} ,
		{"???" ,0x92 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0x93 , &CPU::INDY , &CPU::ILL , 2 , 6 , 0} , 
		{"STY" ,0x94 , &CPU::ZPX , &CPU::STY , 2 , 4 , 0} , 
		{"STA" ,0x95 , &CPU::ZPX , &CPU::STA , 2 , 4 , 0} , 
		{"STX" ,0x96 , &CPU::ZPY , &CPU::STX , 2 , 4 , 0} , 
		{"???" ,0x97 , &CPU::ZPY , &CPU::ILL , 2 , 4 , 0} , 
		{"TYA" ,0x98 , &CPU::IMP , &CPU::TYA , 1 , 2 , 0} , 
		{"STA" ,0x99 , &CPU::ABSY , &CPU::STA , 3 , 5 , 0} , 
		{"TXS" ,0x9A , &CPU::IMP , &CPU::TXS , 1 , 2 , 0} , 
		{"???" ,0x9B , &CPU::ABSY , &CPU::ILL , 3 , 5 , 0} , 
		{"???" ,0x9C , &CPU::ABSX , &CPU::ILL , 3 , 5 , 0} , 
		{"STA" ,0x9D , &CPU::ABSX , &CPU::STA , 3 , 5 , 0} , 
		{"???" ,0x9E , &CPU::ABSY , &CPU::ILL , 3 , 5 , 0} ,
		{"???" ,0x9F , &CPU::ABSY , &CPU::ILL , 3 , 5 , 0} , 

/*************************     0xA0     *************************/	

		{"LDY" ,0xA0 , &CPU::IMM , &CPU::LDY , 2 , 2 , 0} ,
		{"LDA" ,0xA1 , &CPU::INDX , &CPU::LDA , 2 , 6 , 0} ,
		{"LDX" ,0xA2 , &CPU::IMM , &CPU::LDX , 2 , 2 , 0} , 
		{"???" ,0xA3 , &CPU::INDX , &CPU::ILL , 2 , 6 , 0} , 
		{"LDY" ,0xA4 , &CPU::ZP , &CPU::LDY , 2 , 3 , 0} , 
		{"LDA" ,0xA5 , &CPU::ZP , &CPU::LDA , 2 , 3 , 0} , 
		{"LDX" ,0xA6 , &CPU::ZP , &CPU::LDX , 2 , 3 , 0} , 
		{"???" ,0xA7 , &CPU::ZP , &CPU::ILL , 2 , 3 , 0} , 
		{"TAY" ,0xA8 , &CPU::IMP , &CPU::TAY , 1 , 2 , 0} , 
		{"LDA" ,0xA9 , &CPU::IMM , &CPU::LDA , 2 , 2 , 0} , 
		{"TAX" ,0xAA , &CPU::IMP , &CPU::TAX , 1 , 2 , 0} ,
		{"???" ,0xAB , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"LDY" ,0xAC , &CPU::ABS , &CPU::LDY , 3 , 4 , 0} , 
		{"LDA" ,0xAD , &CPU::ABS , &CPU::LDA , 3 , 4 , 0} , 
		{"LDX" ,0xAE , &CPU::ABS , &CPU::LDX , 3 , 4 , 0} , 
		{"???" ,0xAF , &CPU::ABS , &CPU::ILL , 3 , 4 , 0} ,

/*************************     0xB0     *************************/	

		{"BCS" ,0xB0 , &CPU::REL , &CPU::BCS , 2 , 2 , 3} , 
		{"LDA" ,0xB1 , &CPU::INDY , &CPU::LDA , 2 , 5 , 1} ,
		{"???" ,0xB2 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0xB3 , &CPU::INDY , &CPU::ILL , 2 , 5 , 0} , 
		{"LDY" ,0xB4 , &CPU::ZPX , &CPU::LDY , 2 , 4 , 0} , 
		{"LDA" ,0xB5 , &CPU::ZPX , &CPU::LDA , 2 , 4 , 0} ,
		{"LDX" ,0xB6 , &CPU::ZPY , &CPU::LDX , 2 , 4 , 0} , 
		{"???" ,0xB7 , &CPU::ZPY , &CPU::ILL , 2 , 4 , 0} , 
		{"CLV" ,0xB8 , &CPU::IMP , &CPU::CLV , 1 , 2 , 0} ,
		{"LDA" ,0xB9 , &CPU::ABSY , &CPU::LDA , 3 , 4 , 1} ,
		{"TSX" ,0xBA , &CPU::IMP , &CPU::TSX , 1 , 2 , 0} , 
		{"???" ,0xBB , &CPU::ABSY , &CPU::ILL , 3 , 4 , 0} , 
		{"LDY" ,0xBC , &CPU::ABSX , &CPU::LDY , 3 , 4 , 1} ,
		{"LDA" ,0xBD , &CPU::ABSX , &CPU::LDA , 3 , 4 , 1} , 
		{"LDX" ,0xBE , &CPU::ABSY , &CPU::LDX , 3 , 4 , 1} , 
		{"???" ,0xBF , &CPU::ABSY , &CPU::ILL , 3 , 4 , 0} , 

/*************************     0xC0     *************************/	

		{"CPY" ,0xC0 , &CPU::IMM , &CPU::CPY , 2 , 2 , 0} , 
		{"CMP" ,0xC1 , &CPU::INDX , &CPU::CMP , 2 , 6 , 0} ,
		{"???" ,0xC2 , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} ,
		{"???" ,0xC3 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} , 
		{"CPY" ,0xC4 , &CPU::ZP , &CPU::CPY , 2 , 3 , 0} , 
		{"CMP" ,0xC5 , &CPU::ZP , &CPU::CMP , 2 , 3 , 0} , 
		{"DEC" ,0xC6 , &CPU::ZP , &CPU::DEC , 2 , 5 , 0} , 
		{"???" ,0xC7 , &CPU::ZP , &CPU::ILL , 2 , 5 , 0} , 
		{"INY" ,0xC8 , &CPU::IMP , &CPU::INY , 1 , 2 , 0} , 
		{"CMP" ,0xC9 , &CPU::IMM , &CPU::CMP , 2 , 2 , 0} , 
		{"DEX" ,0xCA , &CPU::IMP , &CPU::DEX , 1 , 2 , 0} , 
		{"???" ,0xCB , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"CPY" ,0xCC , &CPU::ABS , &CPU::CPY , 3 , 4 , 0} , 
		{"CMP" ,0xCD , &CPU::ABS , &CPU::CMP , 3 , 4 , 0} , 
		{"DEC" ,0xCE , &CPU::ABS , &CPU::DEC , 3 , 6 , 0} , 
		{"???" ,0xCF , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} , 

/*************************     0xD0     *************************/	

		{"BNE" ,0xD0 , &CPU::REL , &CPU::BNE , 2 , 2 , 3} , 
		{"CMP" ,0xD1 , &CPU::INDX , &CPU::CMP , 2 , 5 , 1} , 
		{"???" ,0xD2 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0xD3 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} , 
		{"???" ,0xD4 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} , 
		{"CMP" ,0xD5 , &CPU::ZPX , &CPU::CMP , 2 , 4 , 0} , 
		{"DEC" ,0xD6 , &CPU::ZPX , &CPU::DEC , 2 , 6 , 0} , 
		{"???" ,0xD7 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} ,
		{"CLD" ,0xD8 , &CPU::IMP , &CPU::CLD , 1 , 2 , 0} , 
		{"CMP" ,0xD9 , &CPU::ABSY , &CPU::CMP , 3 , 4 , 1} , 
		{"???" ,0xDA , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0xDB , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0} ,
		{"???" ,0xDC , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} , 
		{"CMP" ,0xDD , &CPU::ABSX , &CPU::CMP , 3 , 4 , 1} , 
		{"DEC" ,0xDE , &CPU::ABSX , &CPU::DEC , 3 , 7 , 0} , 
		{"???" ,0xDF , &CPU::ABSX , &CPU::ILL , 3 , 7 , 0} , 

/*************************     0xE0     *************************/	

		{"CPX" ,0xE0 , &CPU::IMM , &CPU::CPX , 2 , 2 , 0} , 
		{"SBC" ,0xE1 , &CPU::INDX , &CPU::SBC , 2 , 6 , 0} ,
		{"???" ,0xE2 , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"???" ,0xE3 , &CPU::INDX , &CPU::ILL , 2 , 8 , 0} ,
		{"CPX" ,0xE4 , &CPU::ZP , &CPU::CPX , 2 , 3 , 0} ,
		{"SBC" ,0xE5 , &CPU::ZP , &CPU::SBC , 2 , 3 , 0} ,
		{"INC" ,0xE6 , &CPU::ZP , &CPU::INC , 2 , 5 , 0} , 
		{"???" ,0xE7 , &CPU::ZP , &CPU::ILL , 2 , 5 , 0} , 
		{"INX" ,0xE8 , &CPU::IMP , &CPU::INX , 1 , 2 , 0} , 
		{"SBC" ,0xE9 , &CPU::IMM , &CPU::SBC , 2 , 2 , 0} ,
		{"NOP" ,0xEA , &CPU::IMP , &CPU::NOP , 1 , 2 , 0} , 
		{"???" ,0xEB , &CPU::IMM , &CPU::ILL , 2 , 2 , 0} , 
		{"CPX" ,0xEC , &CPU::ABS , &CPU::CPX , 3 , 4 , 0} , 
		{"SBC" ,0xED , &CPU::ABS , &CPU::SBC , 3 , 4 , 0} , 
		{"INC" ,0xEE , &CPU::ABS , &CPU::INC , 3 , 6 , 0} , 
		{"???" ,0xEF , &CPU::ABS , &CPU::ILL , 3 , 6 , 0} , 

/*************************     0xF0     *************************/	

		{"BEQ" ,0xF0 , &CPU::REL , &CPU::BEQ , 2 , 2 , 3} , 
		{"SBC" ,0xF1 , &CPU::INDY , &CPU::SBC , 2 , 5 , 1} , 
		{"???" ,0xF2 , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0xF3 , &CPU::INDY , &CPU::ILL , 2 , 8 , 0} , 
		{"???" ,0xF4 , &CPU::ZPX , &CPU::ILL , 2 , 4 , 0} , 
		{"SBC" ,0xF5 , &CPU::ZPX , &CPU::SBC , 2 , 4 , 0} , 
		{"INC" ,0xF6 , &CPU::ZPX , &CPU::INC , 2 , 6 , 0} , 
		{"???" ,0xF7 , &CPU::ZPX , &CPU::ILL , 2 , 6 , 0} , 
		{"SED" ,0xF8 , &CPU::IMP , &CPU::SED , 1 , 2 , 0} , 
		{"SBC" ,0xF9 , &CPU::ABSY , &CPU::SBC , 3 , 4 , 1} , 
		{"???" ,0xFA , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} , 
		{"???" ,0xFB , &CPU::ABSY , &CPU::ILL , 3 , 7 , 0} ,
		{"???" ,0xFC , &CPU::ABSX , &CPU::ILL , 3 , 4 , 0} ,
		{"SBC" ,0xFD , &CPU::ABSX , &CPU::SBC , 3 , 4 , 1} ,
		{"INC" ,0xFE , &CPU::ABSX , &CPU::INC , 3 , 7 , 0} , 
		{"???" ,0xFA , &CPU::IMP , &CPU::ILL , 1 , 2 , 0} 	
	};
	
	opcodes_table = table ; 
}



