C++ emulation for the NES

YES... 
I know ... 
People say there are as many NES emulators out there, in the web , as stars in the observable universe. 
I say nonsense! wrong ! 
There's one more to come :)  


What has been done :

The 6502 emulation  , has only the legal opcodes implemented. 
Has a GUI debugger implemented with SFML , that displays : 
- Zero page memory on the top left
- 256 bytes of the bus that you want to display on the bottom left (use + or - on the numerical keyboard to increment/decrement the page)
- The status of the CPU registers on the top right (Accumulator , X , Y , Stack pointer , Program counter) , and the value of the CPU flags
- On the bottom right , the instruction being executed + it's opcode , the relative address for branch instructions and the absolute address . use 'c'
  to go to the next instruction. 


What has been tested ?

The program can take as argument , for now , a machine code program that you can generate from 6502 assembly (like ... online or something... I'm too lazy to implement this shit). 


