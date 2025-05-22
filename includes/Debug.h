#ifndef DEBUG_H
#define DEBUG_H

#include "CPU.h"
#include "Constants.h"
#include "GUI.h"

class Window;

class Debug {
 public:
  virtual std::string print_registers() const = 0;
  virtual void loop() const = 0;
  virtual std::string current_instruction() const = 0;
  virtual std::string print_zeropage() const = 0;
  virtual std::string print_memory(uint8_t) const = 0;
};

class CPUDEBUG : public Debug {
 public:
  CPUDEBUG(CPU *c, Window *w) {
    cpu = c;
    window = w;
  }
  virtual ~CPUDEBUG() {}
  virtual std::string print_registers() const;
  virtual void loop() const;
  virtual void write_log() const;
  virtual std::string current_instruction() const;
  virtual std::string print_zeropage() const;
  virtual std::string print_memory(uint8_t mem_page) const;

  CPU *cpu;
  Window *window;
};

#endif
