#include "../includes/NesParser.h"
#define TRAINER_SIZE 512

namespace NESPARSER {

  /* 0x4E45531A = "NES" + DOS EOF
   *
   */
  NESROM data(const char *file) {
    NESROM rom;
    std::ifstream input(file, std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    bool validNes = buffer.size() > 0;
    uint32_t header = *(uint32_t *)buffer.data();
    validNes &= (header == 0x1A53454E);

    std::cout << "Parsing : " << file << std::endl;
    if (validNes) {
      unsigned int i = 4;
      rom.header.validNes = true;
      rom.header.PRG_ROM = buffer[i++];
      rom.header.CHR_ROM = buffer[i++];
      rom.header.FLAG6 = buffer[i++];
      rom.header.FLAG7 = buffer[i++];
      rom.header.FLAG8 = buffer[i++];
      rom.header.FLAG9 = buffer[i++];
      rom.header.FLAG10 = buffer[i++];
      i += 5;  // unused padding of 5 bytes

      uint8_t trainer = (rom.header.FLAG6 >> 2) & 0x01;  // bit 2 of flag 6 indicates trainer
      if (trainer == 0X01)
        for (int j = 0; j < TRAINER_SIZE; j++, i++)
          rom.trainer.push_back(buffer[i]);
      rom.rom_data.resize(rom.header.PRG_ROM * 16 * 1024);
      for (std::size_t j = 0; j < rom.rom_data.size(); j++, i++) {
        rom.rom_data[j] = buffer[i];
      }

      return rom;
    }
    return rom;
  }

}  // namespace NESPARSER

namespace ASSEMBLY {

  uint8_t getIndex(char c) {
    for (int i = 0; i <= 0xF; i++)
      if (UTILITY::table[i] == c)
        return i;
    return 0;
  }

  // return hex equivalent of string characters
  std::vector<uint8_t> translate(std::string content) {
    std::vector<uint8_t> result;
    for (std::size_t i = 0; i < content.size(); i += 2) {
      uint8_t high = getIndex(content[i]);
      uint8_t low = getIndex(content[i + 1]);
      uint8_t value = (high << 4) | low;
      result.push_back(value);
    }
    for (uint8_t i : result)
      std::cout << UTILITY::format(i) << "  ";
    std::cout << std::endl;

    return result;
  }

  std::vector<uint8_t> data(const char *file) {

    std::ifstream input(file);
    if (input) {
      std::string content;
      char c;
      while (input >> c)
        content += c;
      return translate(content);
    } else {
      std::cerr << file << " not found";
      return std::vector<uint8_t>();
    }
  }

}  // namespace ASSEMBLY

namespace BINARY {

  std::vector<uint8_t> data(const char *file) {
    std::ifstream input(file);
    std::vector<uint8_t> dat;
    if (input) {
      uint8_t value;
      while (input >> value)
        dat.push_back(value);

      return dat;
    } else {
      std::cerr << file << " not found";
      return dat;
    }
  }

}  // namespace BINARY
