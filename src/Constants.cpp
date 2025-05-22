#include "../includes/Constants.h"

namespace UTILITY {

  std::string formatU8(uint8_t val) {
    uint8_t low = val & 0x0F;
    uint8_t high = (val & 0xF0) >> 4;
    char h = table[high];
    char l = table[low];
    return std::string(1, h) + std::string(1, l);
  }

  std::string formatU16(uint16_t val) {
    uint8_t low = val & 0x00FF;
    uint8_t high = (val & 0xFF00) >> 8;
    std::string h = formatU8(high);
    std::string l = formatU8(low);
    return h + l;
  }

  std::string format(uint8_t val) { return std::string("0x") + formatU8(val); }

  std::string format(uint16_t val) { return std::string("0x") + formatU16(val); }

}  // namespace UTILITY
