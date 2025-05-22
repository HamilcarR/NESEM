This repository contains a Nintendo Entertainment System (NES) emulator.

## Core Components

The NES emulator is comprised of several key components that work together to simulate the original hardware:

*   **CPU (Central Processing Unit):** A Ricoh 2A03 processor (based on the MOS Technology 6502 core) that executes game logic. It reads instructions and data from memory via the bus system and controls other components like the PPU and APU (Audio Processing Unit).
*   **PPU (Picture Processing Unit):** Responsible for rendering graphics, including backgrounds and sprites. It has its own dedicated memory (VRAM) for storing nametables (background layout) and pattern tables (tile/sprite graphics), as well as palette RAM for color information. The PPU operates on a separate clock cycle from the CPU and generates the video output.
*   **RAM (Random Access Memory):** The NES has 2KB of internal work RAM (WRAM) used by the CPU for storing game variables, stack data, and other temporary information.
*   **Bus System:** The central communication highway that connects the CPU, PPU, RAM, and the game cartridge. It arbitrates memory access, allowing different components to read and write data to specific memory locations.

### Memory Mapping

The NES uses a memory-mapped I/O system. Here's a simplified overview:

**CPU Memory Map:**
*   `$0000-$07FF`: 2KB Internal RAM (mirrored up to `$1FFF`)
*   `$2000-$2007`: PPU Registers (mirrored up to `$3FFF`) - Used by the CPU to control the PPU.
*   `$4000-$4017`: APU (Audio Processing Unit) and I/O Registers.
*   `$4018-$401F`: Test APU and I/O functionality (normally disabled).
*   `$4020-$FFFF`: Cartridge Space (PRG ROM, PRG RAM, and mapper registers). Game code and data are stored here.

**PPU Memory Map:**
*   `$0000-$1FFF`: Pattern Tables (CHR ROM or RAM from the game cartridge) - Contains tile and sprite graphics data.
*   `$2000-$2EFF`: Nametables (VRAM) - Stores the layout of background tiles. The NES has internal VRAM for two nametables, and cartridges can provide more.
*   `$3F00-$3F1F`: Palette RAM - Stores color information for backgrounds and sprites.

### Component Interaction

1.  The **CPU** fetches game instructions from the cartridge's PRG ROM via the **Bus**.
2.  Based on these instructions, the CPU manipulates game data in **RAM** and controls the **PPU** by writing to its registers (e.g., telling it which background to display or where sprites are).
3.  The **PPU** reads tile and sprite data from the cartridge's CHR ROM/RAM (Pattern Tables) and layout information from its Nametables (VRAM) via the **Bus**.
4.  The **PPU** uses this data and its Palette RAM to render a frame of video, which is then outputted.
5.  The **Bus** handles all data transfers between these components, ensuring data goes to the correct addresses.

## Supporting Modules

Beyond the core hardware emulation, the project includes modules to aid in development and game loading:

*   **GUI for Debugging:** A graphical user interface is provided to help visualize the internal state of the emulator. This includes:
    *   Viewing the current pattern tables and nametables.
    *   Inspecting CPU registers and memory.
    *   Step-by-step execution of CPU instructions.
    *   Displaying the rendered game output.
    This is invaluable for debugging the emulator's behavior and understanding how NES games operate.

*   **NES ROM Parsing (.nes file support):** The emulator can parse standard `.nes` ROM files. This involves:
    *   Reading the iNES header to determine ROM size (PRG and CHR), mapper type, mirroring mode, and other metadata.
    *   Loading Program ROM (PRG ROM) and Character ROM (CHR ROM) data into the emulator's memory space.
    *   Handling different NES cartridge mappers (though initial support might be for simpler mappers like NROM).
    This allows the emulator to load and run a wide variety of NES games.

## Testing and Development

The repository includes several resources to support the development and testing of the emulator:

*   **Makefile:** A standard `Makefile` is provided to automate the build process. It likely contains targets for compiling the emulator, running tests, and cleaning the build directory.
*   **Test Scripts:**
    *   `check_opcode.sh`: This shell script is used for testing the CPU's opcode implementations. It likely runs the emulator with a specific test ROM (like `nestest.nes`) and compares the output against a known good log file (`nestest.log`) to verify instruction correctness.
*   **Test ROMs:**
    *   `nestest.nes`: A widely used NES test ROM that executes a comprehensive suite of 6502 instructions. It's crucial for verifying the accuracy of the CPU emulation. The expected output is often compared against `nestest.log`.
    *   `assembly.asm`: This is likely a custom assembly file containing specific tests or routines, possibly for testing particular PPU behaviors, mapper interactions, or other custom test cases for the emulator.
*   **Font Files:**
    *   `font/font.ttf`: This TrueType font file is likely used by the GUI, perhaps for rendering debug information, text in menus, or other visual elements within the emulator's interface.
*   **Session File:**
    *   `session`: This file might store the state of a debugging session, allowing developers to resume their work or save a particular scenario for later analysis.
*   **Display Script:**
    *   `display.sh`: This script could be used to launch the emulator or a part of its graphical output, possibly configuring display settings or running a specific test view.

These tools and files are essential for ensuring the emulator's accuracy, debugging issues, and streamlining the development workflow.
