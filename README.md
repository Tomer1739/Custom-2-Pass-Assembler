# Custom 2-Pass Assembler

**Author:** Tomer Gresler  
**Course:** 20465 – Systems Programming Lab 1, Open University  
**Language:** C  

---

## Project Overview

**Custom 2-Pass Assembler** is a C-based assembler that translates assembly code into machine code using a **two-pass process**:

1. **Pass 1:** Builds a symbol table and computes addresses.  
2. **Pass 2:** Generates machine code by replacing symbols with their addresses.  

This project demonstrates **system-level programming, modular design, and proficiency in Unix/Linux environments**, all skills relevant to software development, networking, and low-level systems work.

---

## Key Skills Demonstrated

- **C programming & system-level coding**  
- **Modular software design** (clear separation of responsibilities across `.c` files)  
- **Working with Unix/Linux environments** and system calls  
- **Data structures & symbol table management**  
- **Software testing and debugging** (through included sample assembly files)

---

## How to Build & Run

1. Open terminal in the project folder.  
2. Compile:  
```bash
make
Run the assembler:

bash
Copy code
./run1 test
Replace test with your assembly file (test.as or test.am).

Project Structure
File	Purpose
main.c	Program entry point
run1	Executable after compilation
makefile	Build automation
analyzeDirective.c/.h	Handles assembly directives
analyzeOrder.c/.h	Processes instruction order and syntax
convertFunctions.c	Converts instructions to machine code
firstPass.c/.h	Implements the first pass (symbol table creation)
preAssembler.c/.h	Preprocessing of assembly files
validateString.c	Validates labels and strings
writeANDsecondPass.c/.h	Performs second pass and writes output
test.as, test.am	Sample assembly files
test.ob, test.ext, test.ent	Expected output files

Each .c file contains detailed comments explaining its function.
