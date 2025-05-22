#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace UTILITY {

  inline const char *table = "0123456789ABCDEF";
  std::string formatU8(uint8_t val);
  std::string formatU16(uint16_t val);
  std::string format(uint8_t val);
  std::string format(uint16_t val);

}  // namespace UTILITY

#endif
