# Brainfuck to StackVM compiler

This repository is the home of a small compiler written in C. It compiles codes written in the
[Brainfuck Language](https://en.wikipedia.org/wiki/Brainfuck) to assembly code for my own virtual machine.

# Installation

You will need my [StackVM](https://github.com/KCreate/stackvm) to be able to assemble and execute the
resulting assembly file. Take look at the installation instructions over there.

1. `git clone https://github.com/KCreate/brainfuck-compiler`
2. `make`
3. `bin/bfc input.bf > myprog.asm`
4. `stackvm build myprog.asm -s -o myprog.bc`
5. `stackvm run myprog.bc`

# Features

| Command | Meaning                                                                         | Status |
|:-------:|---------------------------------------------------------------------------------|:------:|
| `>`     | Increment data pointer                                                          | ✓      |
| `<`     | Decrement data pointer                                                          | ✓      |
| `+`     | Increment the value in the current cell                                         | ✓      |
| `-`     | Decrement the value in the current cell                                         | ✓      |
| `.`     | Output the byte at the data pointer                                             | ✓      |
| `,`     | Read a byte of input and store it in the byte at the data pointer               | 𐄂      |
| `[`     | Jump to the matching `]` command if the byte at the data pointer is `0`         | ✓      |
| `]`     | If the current byte is nonzero, jump back to the command after the matching `[` | ✓      |

# Todos

- [ ] Simple peep-hole optimizations
- [ ] Better error handling
- [ ] Location information in the AST
- [ ] `,` command (StackVM needs to be patched for this to work)

# Credits

- [Leonard Schütz @KCreate](https://github.com/KCreate)

# Additional resources

- [Brainfuck on Wikipedia](https://en.wikipedia.org/wiki/Brainfuck)
