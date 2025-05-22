#include "../includes/Debug.h"
#include <future>

using namespace UTILITY;

std::string CPUDEBUG::print_registers() const {
  std::ostringstream oss(std::ostringstream::out);
  const CPU::REGISTERS registers = cpu->getRegisters();
  oss << "Registers--------------" << "\n";
  oss << "A: " << format(registers.A) << "\n";
  oss << "X: " << format(registers.X) << "\n";
  oss << "Y: " << format(registers.Y) << "\n";
  oss << "SP: " << format(registers.SP) << "\n";
  oss << "PC: " << format(registers.PC) << "\n";
  oss << "FLAGS--------------" << "\n";
  int N, V, U, B, D, I, Z, C;
  N = cpu->get_flag(CPU::N);
  V = cpu->get_flag(CPU::V);
  U = cpu->get_flag(CPU::U);
  B = cpu->get_flag(CPU::B);
  D = cpu->get_flag(CPU::D);
  I = cpu->get_flag(CPU::I);
  Z = cpu->get_flag(CPU::Z);
  C = cpu->get_flag(CPU::C);
  oss << "N V U B D I Z C" << "\n";
  oss << N << " " << V << " " << U << " " << B << " " << D << " " << I << " " << Z << " " << C << std::endl;

  return oss.str();
}

std::string CPUDEBUG::current_instruction() const {
  std::ostringstream oss(std::ostringstream::out);

  const CPU::INSTRUCTION instruction = cpu->opcodes_table[cpu->current_opcode];
  oss << "INSTRUCTION--------------" << "\n";
  oss << "Op: " << instruction.mnemonic << "  " << format(instruction.opcode) << "\n";
  oss << "Data: " << format(cpu->data) << " count:" << (int)cpu->instruction_count << "\n";
  oss << "Address absolute: " << format(cpu->abs_addr) << "\n";
  oss << "Address relative: " << format(cpu->rel_addr) << std::endl;
  return oss.str();
}

std::string CPUDEBUG::print_zeropage() const {
  std::ostringstream oss(std::ostringstream::out);
  uint16_t addr = 0x0000;
  uint8_t value = 0;
  for (uint16_t k = 0x0000; k <= 0x00F0; k += 0x0010) {
    uint8_t i = k & 0x00FF;
    if (k > 0x00F0)
      break;
    addr = i;
    oss << format(addr) << ":     ";
    for (uint8_t j = 0; j <= 0x0F; j++) {
      uint8_t z = i | j;
      value = cpu->getBus()->read(addr | (z & 0x00FF));
      oss << format(value) << "   ";
    }
    oss << "\n";
  }

  oss << std::endl;
  return oss.str();
}

std::string CPUDEBUG::print_memory(uint8_t mempage) const {

  std::ostringstream oss(std::ostringstream::out);
  uint16_t temp = (mempage & 0x00FF) << 8;
  uint16_t addr;
  uint8_t value = 0;
  for (uint16_t k = 0x0000; k <= 0x00F0; k += 0x10) {
    uint8_t i = k & 0x00FF;
    if (k > 0x00F0)
      break;
    addr = temp | i;
    oss << format(addr) << ":     ";
    for (uint8_t j = 0; j <= 0x0F; j++) {
      uint8_t z = i | j;
      value = cpu->getBus()->read(addr | (z & 0x00FF));
      oss << format(value) << "   ";
    }
    oss << "\n";
  }

  oss << std::endl;
  return oss.str();
}

void CPUDEBUG::write_log() const {}

void CPUDEBUG::loop() const {
  cpu->power_on();
  window->loop(this);
}
