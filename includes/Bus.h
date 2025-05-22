#ifndef BUS_H
#define BUS_H
#include "CPU.h"
#include "Constants.h"

class CPU;

class BUSBASIC {
 public:
  virtual uint8_t read(uint16_t address) const = 0;
  virtual void write(uint16_t address, uint8_t value) = 0;

 private:
  virtual void mirror(uint16_t address, uint8_t value) = 0;
  virtual bool isMirrored(uint16_t address) const = 0;
};

// CPU BUS
class BUS : public BUSBASIC {
 public:
  struct IRVECT {
    static const uint16_t IRQ_INTERRUPT_VECTOR_LOW = 0xFFFE;
    static const uint16_t IRQ_INTERRUPT_VECTOR_HIGH = 0xFFFF;
    static const uint16_t RES_VECTOR_LOW = 0xFFFC;  // reset
    static const uint16_t RES_VECTOR_HIGH = 0xFFFD;
    static const uint16_t NMI_INTERRUPT_VECTOR_LOW = 0xFFFA;
    static const uint16_t NMI_INTERRUPT_VECTOR_HIGH = 0xFFFB;
  };

  struct DEVICES {
    CPU *cpu;
  };

  struct STACK {
    static const uint16_t BEGIN = 0x0100;
    static const uint16_t END = 0x01FF;
  };
  struct PRGROM {
    static const uint16_t BEGIN = 0x8000;
    static const uint16_t BANK2_BEGIN = 0xC000;
  };
  // Addresses in range 0x0000 - 0x07FFF are mirrored in the range 0x8000 - 0x1FFF every 0x0800 bytes
  struct MIRROR {
    static const uint16_t A_BEGIN = 0x0000;
    static const uint16_t A_END = 0x07FF;

    static const uint16_t B_BEGIN = 0x0800;
    static const uint16_t B_END = 0x0FFF;

    static const uint16_t C_BEGIN = 0x1000;
    static const uint16_t C_END = 0x17FF;

    static const uint16_t D_BEGIN = 0x1800;
    static const uint16_t D_END = 0x1FFF;

    static const uint16_t PADDING = 0x0800;
  };

  // PPU registers , mirrored and can be modified by the CPU
  struct PPUREG {
    static const uint16_t BEGIN = 0x2000;
    static const uint16_t END = 0x2007;

    /* PPUCTRL : Controller $2000 : write (PPU control register)
     * Bits : VPHB SINN
     *
     * V : NMI Enable : Generate an NMI at the start of the vertical blanking interval
     * 			( 0 = off ; 1 = on )
     *
     * P : PPU master/slave select				//NOT USED IN NES
     * 			( 0 = read backdrop from EXT pins , 1 = output color on EXT pins)
     *
     * H : Sprite height , Sprite size
     * 			( 0 = 8x8 pixels , 1 = 8x16 pixels)
     *
     * B : Background tile select , Background pattern table address
     * 				( 0 = $0000 , 1 = $1000)
     *
     * S : Sprite tile select , Sprite pattern table address for 8x8 sprites
     * 			    ( 0 = $0000 , 1 = $1000 , ignored in 8x16 mode)
     *
     * I : Increment mode , VRAM address increment per CPU read/write of PPUDATA
     * 			( 0 = add 1 , going through y ; 1 = add 32 , going down +x )
     *
     *			Name Table , I = 0 		Name Table , I = 1
     *			+----------------+		+----------------+
     *			|-----> write			| | write
     *			|				| |
     *			+				+ V
     *
     *
     * NhNl : Nametable select ( 00 = $2000 , 01 = $2400 , 10 = $2800 , 11 = $2C00 )
     *			(Nh : 1 = add 240 to Y scroll position , Nl : 1 = add 256 to X scroll position)
     *
     *			+-----------------+--------------+
     *			|2  ($2800)	  |3  ($2C00)	 |
     *			+-----------------+--------------+
     *			|0  ($2000	  |1  ($2400)	 |
     *			+-----------------+--------------+
     */
    static const uint16_t PPUCTRL = 0x2000;

    /* PPUMASK : MASK $2001 : write ( Controls the rendering of sprites and background and color effects)
     * Bits : BGRs bMmG
     * B : Emphasize blue background color
     * G : Emphasize green background
     * R : Emphasize red background
     * s : Show sprites if = 1
     * b : Show background if = 1
     * M : 1 = Show sprites in leftmost 8 pixels of screen , 0 = hide
     * m : 1 = Show background in leftmost 8 pixels of screen , 0 = hide
     * G : Greyscale ( 0 : normal colors , 1 : greyscale display)
     */
    static const uint16_t PPUMASK = 0x2001;

    /* PPUSTATUS : Status $2002 : read : ( reflects the state of functions inside PPU)
     * Bits : VSOY YYYY
     * V : Vertical blank has started ( 0 : not in vblank , 1 : in vblank )
     * 			Is cleared after a read .TODO
     *
     * S : Sprite 0 Hit : Set when a nonzero pixel of sprite 0 overlaps a nonzero background pixel ,
     * 			cleared at dot 1 of the pre-render line , used for raster timing
     *
     * O : Sprite overflow : set when more than 8 sprites on a scanline , but bugged : TODO
     *
     * YYYYY: Least significant bits previously written into a PPU register
     * 		(register not being updated for this address)
     */
    static const uint16_t PPUSTATUS = 0x2002;

    /* OAMADDR : OAM ADDRESS $2003 : write
     * OAM address port
     * Write the address of OAM you want to access here .
     *
     * VALUES WHEN RENDERING :
     * Is set to 0 during each ticks 257-320 (sprite tile loading interval) of the pre-render and visible scanlines
     * The value when sprite evaluation starts at tick 65 of the visible scanline determine where in OAM sprite
     * evaluation starts , and which sprite gets treated as sprite 0 .
     * The first OAM entry to be checked is the one starting at OAM[OAMADDR].
     * If OAMADDR is not aligned and doesn't point to the Y position (first byte) of an OAM entry , then
     * whatever it points to (tile index , attrib , X coord) will be reinterpreted as Y position and the following
     * bytes will be similarly reinterpreted .
     * No more Sprites will be found once the end of OAM is reached , effectively hiding any sprites before
     * OAM[OAMADDR].
     *
     * BUGS:
     * If OAMADDR is less than 8 when rendering starts , the 8 bytes starting at OAMADDR & 0xFB are copied to the
     * first eight bytes of OAM .
     */
    static const uint16_t OAMADDR = 0x2003;

    /* OAMDATA : OAM DATA $2004 : read-write
     * Write OAM data , write increment OAMADDR after the write .
     * reads during vertical or forced blanking return the value from OAM at that address but do not increment
     * Ignore writes during rendering
     */
    static const uint16_t OAMDATA = 0x2004;

    /* PPUSCROLL : PPU SCROLLING POSITION REGISTER $2005 : write x2
     * Used to change the scroll position, that is , to tell the PPU which pixel of the nametable selected through PPUCTRL
     * should be at the top left corner of the rendering screen .
     * This register is written to during vertical blanking so that the next frame starts rendering from the desired location
     * but can also be modified during rendering in order to split the screen .
     * Changes made to the vertical scroll during rendering will only take effect the next frame .
     */
    static const uint16_t PPUSCROLL = 0x2005;

    /* PPUADDR : PPU ADDRESS REGISTER $2006 : write x2
     * Used to set the address of the PPU memory to be accessed via $2007 ,
     * the first write to this register will set 8 lower address bits.
     * The second will set 6 upper bits , address increment by 1 , or 32 after each access to $2007 .
     */
    static const uint16_t PPUADDR = 0x2006;

    /* PPUDATA : PPU MEMORY DATA $2007 : write read
     * Used to read/write PPU memory , the address is set via $2006 and increments each access , either by 1 or 32
     *
     */
    static const uint16_t PPUDATA = 0x2007;

    /* OAMDMA : DMA ACCESS TO SPRITE MEMORY $4014 : write
     * Writing a value N into this port causes an area of CPU memory at address $100*N to be transferred into the sprite memory
     *
     */
    static const uint16_t OAMDMA = 0x4014;  // DMA register
  };

  struct PPUREGMIRROR {
    static const uint16_t BEGIN = 0x2008;
    static const uint16_t END = 0x3FFF;
    static const uint8_t PADDING = 0x08;
  };

  BUS() {
    for (std::size_t i = 0; i < bus_size; i++) {
      _bus[i] = 0x00;
    }
    write(IRVECT::RES_VECTOR_LOW, 0x00);
    write(IRVECT::RES_VECTOR_HIGH, 0x80);
  }
  virtual ~BUS() {}

  uint8_t read(uint16_t address) const;                                          // return the value of any memory address within bus range
  void write(uint16_t address, uint8_t value);                                   // write value to any memory address
  void write_stack(uint8_t sp, uint8_t value);                                   // write only to stack , provide only the stack pointer addr
  uint8_t read_stack(uint8_t sp);                                                // read the value pointing by the stack pointer
  void init_rom(std::vector<uint8_t> buffer, uint16_t address = PRGROM::BEGIN);  // rom initialisation
  uint16_t get_reset_vector();                                                   // reads reset vector values on memory

 private:
  static const std::size_t bus_size = 64 * 1024;
  std::array<uint8_t, bus_size> _bus;
  DEVICES devices;
  STACK _stack;  // descending stack

  bool isMirrored(uint16_t address) const;
  void mirror(uint16_t address, uint8_t value);
};

// VRAM
class PPUBUS : public BUSBASIC {
 public:
  struct PATTERNTABLES {
    static const uint16_t P_TABLE_0_BEGIN = 0x0000;
    static const uint16_t P_TABLE_0_END = 0x0FFF;

    static const uint16_t P_TABLE_1_BEGIN = 0x1000;
    static const uint16_t P_TABLE_1_END = 0x1FFF;
  };

  struct NAMETABLES {
    static const uint16_t N_TABLE_0_BEGIN = 0x2000;  // name tables
    static const uint16_t N_TABLE_0_END = 0x23BF;

    static const uint16_t A_TABLE_0_BEGIN = 0x23C0;  // attribute tables
    static const uint16_t A_TABLE_0_END = 0x23FF;

    static const uint16_t N_TABLE_1_BEGIN = 0x2400;
    static const uint16_t N_TABLE_1_END = 0x27BF;

    static const uint16_t A_TABLE_1_BEGIN = 0x27C0;
    static const uint16_t A_TABLE_1_END = 0x27FF;

    static const uint16_t N_TABLE_2_BEGIN = 0x2800;
    static const uint16_t N_TABLE_2_END = 0x2BBF;

    static const uint16_t A_TABLE_2_BEGIN = 0x2BC0;
    static const uint16_t A_TABLE_2_END = 0x2BFF;

    static const uint16_t N_TABLE_3_BEGIN = 0x2C00;
    static const uint16_t N_TABLE_3_END = 0x2FBF;

    static const uint16_t A_TABLE_3_BEGIN = 0x2FC0;
    static const uint16_t A_TABLE_3_END = 0x2FFF;
  };

  struct PALETTES {
    static const uint16_t IMG_PALETTE_BEGIN = 0x3F00;
    static const uint16_t IMG_PALETTE_END = 0x3F0F;

    static const uint16_t SPRITE_PALETTE_BEGIN = 0x3F10;
    static const uint16_t SPRITE_PALETTE_END = 0x3F1F;
  };

  struct MIRRORS {
    static const uint16_t NT_BEGIN = 0x3000;  // nametables mirror range
    static const uint16_t NT_END = 0x3EFF;

    static const uint16_t PT_BEGIN = 0x3F20;  // Palettes mirror range
    static const uint16_t PT_END = 0x3FFF;

    static const uint16_t WRAP_BEGIN = 0x4000;  // PPU is supposed to have only 16kb of memory.
    static const uint16_t WRAP_END = 0xFFFF;    // Addresses beyond 0x3FFF are wrapped around on the hardware memory
                                              // so we extend memory to simuate that behaviour.
                                              // Everything beyond 0x4000 is supposed to be wrapped around from
                                              // 0x0000
  };

  PPUBUS() {
    for (std::size_t i = 0; i < bus_size; i++)
      _bus[i] = 0x00;
  }

  virtual ~PPUBUS() {}
  uint8_t read(uint16_t address) const;
  void write(uint16_t address, uint8_t value);

 private:
  static const std::size_t bus_size = 64 * 1024;
  std::array<uint8_t, bus_size> _bus;  // PPU's own memory

  bool isMirrored(uint16_t address) const;
  void mirror(uint16_t address, uint8_t value);
};

#endif
