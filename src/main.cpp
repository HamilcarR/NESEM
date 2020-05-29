#include "../includes/Constants.h"
#include "../includes/CPU.h" 
#include "../includes/Debug.h"
#include "../includes/NesParser.h" 


using namespace std; 



int main(int argc , char** argv){
	
//	NESROM rom = NESPARSER::data(argv[1]);	
//	auto assembly = ASSEMBLY::data(argv[1]); 
	
	std::vector<uint8_t> buffer = {
		0xA9, 0xA0 , 0xAE , 0x00 ,0x80 , 0x7D , 0x00 , 0x80 , 0x00
	};


	BUS bus= BUS();
	bus.init_rom(buffer) ; 
	CPU cpu=CPU(&bus) ;
	CPUDEBUG debug = CPUDEBUG(&cpu);
	debug.loop() ; 



return EXIT_SUCCESS ; 
}
