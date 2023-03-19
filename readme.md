![LowFish](Logo.png "LowFish") 
<u><b>LowFish a compiled language.</b> </u>
## LOWFISH:
 - Fast compiler. 
 - It is compiled to x86 intel assembly (will not work on ARM chips)
## Features:
Basically a simpler version of C but with Containers. Containers are like namespaces but without any scope. A container only effects how the code can be imported.

## COMPILE STEPS
```c
1. Lexing. Turns program into sequence of Tokens.
2. Parsing. Uses recursive decent parsing to create an AST out of the tokens.
3. IR. Intermedate representation is created out of the code.
4. Code generation. Create assembly code out of the Intermediate Representation.
```
- [x] What?
