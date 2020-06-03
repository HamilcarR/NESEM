#include "../includes/Constants.h"
#include "../includes/CPU.h" 
#include "../includes/Debug.h"
#include "../includes/NesParser.h" 
#include "../includes/GUI.h"

using namespace std; 



int main(int argc , char** argv){
	
//	NESROM rom = NESPARSER::data(argv[1]);	
	auto assembly = ASSEMBLY::data(argv[1]); 
	for(uint8_t i : assembly)
		std::cout << UTILITY::format(i) << "  " ;
	std::cout << std::endl;
	Window window = Window();
	BUS bus= BUS();
	bus.init_rom(assembly) ; 
	CPU cpu=CPU(&bus) ;
	CPUDEBUG debug = CPUDEBUG(&cpu , &window);
	debug.loop() ; 



return EXIT_SUCCESS ; 
}
