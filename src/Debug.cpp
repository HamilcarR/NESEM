#include "../includes/Debug.h"
#include <future>



using namespace UTILITY ; 

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

			case 'c' :
				cpu->clock(); 
				*c = ' ';
			break;
	  }
	do{
	  cpu->clock(); 
	}
	while(cpu->ticks != 0) ;   

	}



}