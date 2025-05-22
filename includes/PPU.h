#ifndef PPU_H
#define PPU_H

#include "Bus.h"
#include "CPU.h"
#include "Constants.h"

class PPUBUS;
class BUS;

class PPU {

 public:
  // all the PPU registers
  struct Registers {
    uint8_t PPUCTRL;    //$2000
    uint8_t PPUMASK;    //$2001
    uint8_t PPUSTATUS;  //$2002
    uint8_t OAMADDR;    //$2003
    uint8_t OAMDATA;    //$2004
    uint8_t PPUSCROLL;  //$2005
    uint8_t PPUADDR;    //$2006
    uint8_t PPUDATA;    //$2007
    uint8_t OAMDMA;     //$2008
  };

  PPU(BUS *cpubus, PPUBUS *ppubus) {
    cpu_bus = cpubus;
    ppu_bus = ppubus;
  }

  virtual ~PPU() {}

  void clock();
  bool in_vblank() const { return vertical_blank; }
  uint8_t readPPUCTRL() const;
  uint8_t writePPUCTRL(uint8_t);
  auto get_X_coord() { return (uint16_t)(ticks / 341 - 21); }
  auto get_Y_coord() { return (uint16_t)(ticks % 341); }
  void enableV() { vertical_blank = true; }
  void disableV() { vertical_blank = false; }
  void step_address();  // use this function to increment address

  /*useful variables*/

  uint16_t address;  // address on the PPU bus
  bool vertical_blank;
  uint16_t ticks;         // need 341 for a scanline
  uint16_t scanline;      // X coord
  uint16_t offset_pixel;  // Y coord
 private:
  BUS *cpu_bus;
  PPUBUS *ppu_bus;
  Registers registers;
};

#endif
