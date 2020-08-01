# CPU
- 8 different 8bit registers (a b c d e f h l)
- some instructions let you read 2 registers at the same time (16 bits)
    - af, bc, de, hl
- flags register (f)
    - lower 4 bits are always zero (why?), upper 4 bits flag certain states 
    - 7 6 5 4 3 2 1 0
    - Z N H C 0 0 0 0
    - Zero, subtract, half carry, carry [(page 62)](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf)
- 2 16 bit registers sp and pc
    - sp: initialized to 0x100
    - pc: initialized to 0xFFFE

# Function pointers
- [Function pointers in a nutshell](https://www.youtube.com/watch?v=ynYtgGUNelE)
- [Function pointers for emudev](http://www.multigesture.net/wp-content/uploads/mirror/zenogais/FunctionPointers.htm)
- [Instruction set map](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html) (super useful!!!)
- With a function pointer table, you can just call instruction_table[opcode] instead of having a giant switch case (or, god forbid, an if else chain)