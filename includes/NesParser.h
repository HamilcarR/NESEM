#ifndef NESPARSER_H
#define NESPARSER_H


#include "Constants.h"


//reads a NES rom
namespace NESPARSER{

struct NESHEADER{
	bool validNes = false ; 
	uint8_t PRG_ROM ; // size of PRG ROM in 16KB units
	uint8_t CHR_ROM ; // 8KB units , 0 = using CHR RAM
	uint8_t	FLAG6 ;  
	uint8_t FLAG7 ; 
	uint8_t FLAG8 ; 
	uint8_t FLAG9 ; 
	uint8_t FLAG10 ;
	


};

struct NESROM{
	NESHEADER header ; 
	std::vector<uint8_t> trainer ;
	std::vector<uint8_t> rom_data ; 

};


NESROM  data(const char* file); 


}


//reads an assembly binary file
namespace ASSEMBLY {

	std::vector<uint8_t> data(const char* file) ;
	

}


namespace BINARY {

	std::vector<uint8_t> data(const char* file); 


}








#endif
