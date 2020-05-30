#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <fstream>





 namespace UTILITY {


 static const char *table = "0123456789ABCDEF" ;
 std::string formatU8(uint8_t val);
 std::string formatU16(uint16_t val);
 std::string format(uint8_t val);
 std::string format(uint16_t val);



}


#endif 
