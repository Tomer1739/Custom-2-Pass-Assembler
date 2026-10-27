# Custom 2-Pass Assembler

Final project for course 20465 (Systems Programming Lab) at the Open University.
Written in C by Tomer Gresler.

The program is an assembler for a custom assembly language. It reads a `.as` source file
and turns it into machine code. It works in three steps: a pre-assembler that expands
macros, a first pass, and a second pass.

## Building

The project is built with the makefile:

```
make
```

This runs:

```
gcc -g -Wall main.c -o run1 -lm -pedantic -ansi
```

Only `main.c` is passed to gcc. The other `.c` files are pulled in with `#include` from
inside the code (`main.c` includes `preAssembler.c` and `firstPass.c`, and those include
the rest), so they don't need to be listed.

The output is an executable called `run1`. There is a `run1` already in the repo, but it is
a 32-bit build, so it is better to run `make` again on your own machine.

## Running

Give the file names without the `.as` extension. You can give more than one.

```
./run1 test
./run1 file1 file2 file3
```

For each name the program opens `<name>.as`. If it can't open it, it prints an error and
moves on to the next file.

At the end it prints one of these lines:

```
FILE test HAS BEEN PROCCESSED SUCCSEFULLY.
FILE test WAS NOT PROCCESSED, HAS ERRORS.
```

If there were errors, the output files for that file are deleted.

## Output files

For an input `test.as` the program creates:

| File | What it has |
|------|-------------|
| `test.am` | The source after macros are expanded. Comment lines (starting with `;`) and empty lines are removed. |
| `test.txt` | The words from the first pass, in binary (0 and 1). Label operands are still written as names. Data words start with `D`. This file is kept on purpose so you can see the code before the final translation. |
| `test.ob` | The final machine code. First line is the instruction count and the data count. Then one line per word: address and 14 bits, where `/` is 1 and `.` is 0. |
| `test.ent` | Labels declared with `.entry` and their addresses. Only created if there are entries. |
| `test.ext` | Every place an external label is used, and the address where it is used. Only created if external labels are used. |

While running, the program also makes `testent.txt` and `testext.txt` to hold the entry and
extern names. They are deleted at the end.

If you run the assembler again on the same name, the old output files are deleted first.

## The language

These are the things the code handles.

Instructions (16, opcodes 0 to 15):

```
mov cmp add sub not clr lea inc dec jmp bne red prn jsr rts stop
```

Registers: `r0` to `r7`.

Operand types:
- Immediate: `#5`, `#-1`
- Direct (a label): `LENGTH`
- Jump with parameters: `L1(#-1,r6)` (used with `jmp`, `bne`, `jsr`, no spaces inside the parentheses)
- Register: `r3`

Directives:
- `.data 6,-9,15` - list of integers
- `.string "abcdef"` - string, stored one char per word plus a 0 at the end
- `.entry NAME` - label that other files can use
- `.extern NAME` - label defined in another file

Labels end with `:`, must start with a letter, can be up to 30 characters, and can't be an
instruction name or a register name.

Macros start with `mcr <name>` and end with a line `endmcr`. Where the macro name is used,
the pre-assembler puts the macro content instead.

Every line except `stop` and `rts` must have a space in it (between the instruction and the
operands).

## Example

This is the `test.as` file in the repo:

```
; file ps.as
.entry LENGTH
.extern W
MAIN: mov r3 ,LENGTH
LOOP: jmp L1(#-1,r6)
prn #-5
bne W(r4,r5)
sub r1, r4
bne L3
L1: inc K
.entry LOOP
bne LOOP(K,W)
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
.extern L3
```

Running `./run1 test` gives these files (shortened).

`test.txt` (after the first pass). The line `MAIN: mov r3 ,LENGTH` became three words, and the
third one is still the label name:

```
100 00000000110100
101 00001100000000
102 LENGTH
...
```

`test.ob` (after the second pass). `LENGTH` was replaced by its address (130), and the
words are written with `/` and `.`:

```
     23   11
0100 ........//./..
0101 ....//........
0102 ..../....././.
0103 ..///../../...
...
0131 //////////.///
0132 ..........////
0133 ........././/.
```

The first line says there are 23 instruction words and 11 data words. The code starts at
address 100. The data is placed after all the instructions.

`test.ent`:

```
LENGTH 130
LOOP 103
```

`test.ext`:

```
W  110
L3  115
W  121
```

## What each step does

**Pre-assembler** (`preAssembler.c`). Reads the `.as` file and finds all the macros. It saves
each macro name and its content in a linked list. Then it writes the `.am` file, where macro
calls are replaced by the content, and comments and empty lines are left out.

**First pass** (`firstPass.c`, `analyzeOrder.c`, `analyzeDirective.c`). Reads the `.am` file
line by line, starting at address 100. Labels go into a linked list with their address.
Each instruction is turned into a first word (opcode and operand types) plus extra words for
the operands. Numbers and registers are turned into binary right away. If an operand is a
label, only its name is written for now, because its address may not be known yet.
`.data` and `.string` become data words. `.entry` and `.extern` names are saved to temp
files. Syntax errors are printed with the line number. Everything is written to the `.txt` file.

**Second pass** (`writeANDsecondPass.c`). Reads the `.txt` file again. For each word:
- If it is a label name, it looks up the address in the label list and writes it.
- If it is a name that is not in the list, it is treated as an extern. It writes a word with only the
  external bit set and adds a line to the `.ext` file.
- Instruction words are written as they are.

After the instructions it writes all the data words. Then it writes the `.ent` file, and checks
that an extern label is not also defined in the file or also declared as entry.

## File structure

| File | What it does |
|------|--------------|
| `main.c` | Entry point. Loops over the file names, runs the steps, deletes old and temp files. |
| `preAssembler.c` / `.h` | Macro expansion, creates the `.am` file. |
| `firstPass.c` / `.h` | First pass. Label list, label checks, counts IC and DC. |
| `analyzeOrder.c` / `.h` | Turns instruction lines into words. Has the tables of instruction and register names. |
| `analyzeDirective.c` / `.h` | Turns `.data` and `.string` into words, saves `.entry` and `.extern` names. |
| `convertFunctions.c` | Helpers to convert numbers to binary and to check if a string is an integer. |
| `validateString.c` | Syntax checks for `.data`, `.string`, instructions, `#` operands, and extern/entry conflicts. |
| `writeANDsecondPass.c` / `.h` | Writes the `.txt` file, and does the second pass that creates `.ob`, `.ent`, `.ext`. |
| `makefile` | Builds `run1`. |
| `run1` | A compiled build of the program. |
| `test.as` | Example input. |
| `test.am`, `test.txt`, `test.ob`, `test.ent`, `test.ext` | Output from running `./run1 test`. |
