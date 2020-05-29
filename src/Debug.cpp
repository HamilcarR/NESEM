#include "../includes/Debug.h"
#include <future>


const char *table ="0123456789ABCDEF";

static std::string formatU8(uint8_t val){
	uint8_t low = val & 0x0F ; 
	uint8_t high = (val & 0xF0) >> 4 ; 
	char h = table[high] ; 
	char l = table[low] ; 
	return  std::string(1 , h) + std::string(1 , l); 


}


static std::string formatU16(uint16_t val){
	uint8_t low = val & 0x00FF ; 
	uint8_t high = (val & 0xFF00) >> 8 ; 
	std::string h = formatU8(high) ; 
	std::string l = formatU8(low) ; 	
	return h + l; 


}

static std::string format(uint8_t val){

	return std::string("0x") + formatU8(val) ; 
}


static std::string format(uint16_t val){

	return std::string("0x") + formatU16(val); 
}



void CPUDEBUG::print_registers() const {
	const CPU::REGISTERS registers = cpu->getRegisters(); 
	std::cout << "Registers--------------" << std::endl; 
	std::cout << "A: " << format(registers.A) << "\n"; 
	std::cout << "X: " << format(registers.X) << "\n"; 
	std::cout << "Y: " << format(registers.Y) << "\n";
	std::cout << "SP: " << format(registers.SP) << "\n";
	std::cout << "PC: " << format(registers.PC) << "\n"; 
	std::cout << "FLAGS--------------" << std::endl; 
	int N , V , U , B , D , I , Z , C ; 
	N = cpu->get_flag(CPU::N) ; 
	V = cpu->get_flag(CPU::V) ; 
	U = cpu->get_flag(CPU::U) ; 
	B = cpu->get_flag(CPU::B) ; 
	D = cpu->get_flag(CPU::D) ; 
	I = cpu->get_flag(CPU::I) ; 
	Z = cpu->get_flag(CPU::Z) ; 
	C = cpu->get_flag(CPU::C) ; 
	std::cout << "N V U B D I Z C" << "\n";
	std::cout << N << " " << V << " " << U << " " << B << " " << D << " " << I << " " << Z << " " << C << "\n" ; 


}

void CPUDEBUG::current_instruction() const {
	const CPU::INSTRUCTION instruction = cpu->opcodes_table[cpu->current_opcode] ; 	
	std::cout << "INSTRUCTION--------------" << std::endl; 
	std::cout << "Op: " << instruction.mnemonic << "  " << format(instruction.opcode) << "\n" ; 
	std::cout << "Data: " << format(cpu->data) << "\n" ; 
	std::cout << "Address absolute: " << format(cpu->abs_addr) << "\n" ; 


}













void CPUDEBUG::loop() const {
	char *c = new char[1]  ;
	*c = ' ' ; 
	//add thread
	auto keyboard_input=[](char* c) { while(*c !='q') std::cin >> c ; } ; 
	cpu->power_on(); 
	auto a = std::async(std::launch::async , keyboard_input , c ) ; 

	while(*c != 'q'){

		switch (*c) {
			case 'r' :
				print_registers(); 
				std::cout << "clock : " << cpu->ticks << "\n" ; 
				*c = ' ';
			break ; 

			case 'i' :
				current_instruction();
				std::cout << "clock : " << cpu->ticks << "\n" ;
				*c = ' '; 
			break;
	  }

	  cpu->clock(); 
	   

	}



}
