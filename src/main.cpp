#include "../includes/CPU.h"
#include "../includes/Constants.h"
#include "../includes/Debug.h"
#include "../includes/GUI.h"
#include "../includes/NesParser.h"

using namespace std;

int main(int argc, char **argv) {

#ifdef DEBUG
  std::ofstream out("emu.log", std::ios::trunc);
  out.close();
#endif

  auto rom = NESPARSER::data(argv[1]).rom_data;
  //	auto rom = ASSEMBLY::data(argv[1]);
  //	auto rom = BINARY::data(argv[1]);
  Window window = Window();
  BUS bus = BUS();
  bus.init_rom(rom, 0xC000);
  CPU cpu = CPU(&bus);
  CPUDEBUG debug = CPUDEBUG(&cpu, &window);
  debug.loop();

  return EXIT_SUCCESS;
}
