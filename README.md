# C++ Compiler Series

A comprehensive compiler implementation project developed for CSE 340 (Principles of Programming Languages), demonstrating the complete compiler pipeline from lexical analysis to code execution.

## Overview

This repository contains three progressive compiler projects that build upon each other, implementing increasingly sophisticated language processing capabilities. Each project represents a critical phase in the compilation process, from basic parsing to intermediate code generation and execution.

## Project Structure

### Project 1: Polynomial Language Compiler
A specialized compiler for processing polynomial expressions with semantic analysis capabilities.

**Features:**
- Custom lexical analyzer for tokenizing polynomial syntax
- Recursive descent parser for polynomial declarations and expressions
- Semantic analysis detecting:
  - Duplicate polynomial declarations
  - Invalid monomials
  - Undeclared polynomial usage
  - Argument count mismatches
- Polynomial evaluation engine with parameter substitution
- Data flow analysis for detecting:
  - Uninitialized variables (Warning Code 1)
  - Useless assignments (Warning Code 2)
- Expression tree construction for arithmetic operations (ADD, SUB, MUL, POW)

**Key Components:**
- `lexer.h/cc` - Tokenization and keyword recognition
- `parser.h/cc` - Syntax parsing and semantic validation (~850 lines)
- `inputbuf.h/cc` - Input buffer management

### Project 2: Grammar Analyzer
A formal grammar processing system for context-free grammars.

**Features:**
- Grammar specification parser
- Terminal and non-terminal symbol identification
- FIRST set computation for predictive parsing
- FOLLOW set generation
- Ambiguity detection in production rules
- Grammar classification algorithms
- Support for epsilon productions

**Key Components:**
- `project2.cc` - Grammar analysis implementation (~710 lines)
- `lexer.h/cc` - Token processing for grammar notation

### Project 3: Intermediate Code Generator & Executor
A complete compiler backend generating and executing intermediate representation (IR) code.

**Features:**
- IR code generation for imperative language constructs
- Virtual machine execution engine
- Support for control flow structures:
  - Conditional jumps (CJMP)
  - Unconditional jumps (JMP)
  - Switch statements
  - For loops
- Arithmetic operations (ADD, SUB, MUL, DIV)
- I/O instructions (IN, OUT)
- Memory management with location table
- Variable scope handling

**Key Components:**
- `demo.cc` - Parser and IR generation (~408 lines)
- `execute.h/cc` - Virtual machine execution engine
- `lexer.h/cc` - Lexical analysis

## Technical Highlights

### Language Processing Pipeline
1. **Lexical Analysis** - Tokenization with lookahead support
2. **Syntax Analysis** - LL(1) recursive descent parsing
3. **Semantic Analysis** - Type checking and error detection
4. **IR Generation** - Three-address code emission
5. **Execution** - Direct IR interpretation

### Advanced Features
- **Data Flow Analysis** - Forward/backward analysis for optimization warnings
- **Memory Management** - Dynamic memory allocation with symbol tables
- **Error Recovery** - Comprehensive error reporting with line numbers
- **Modular Design** - Clean separation of concerns across compilation phases

## Technologies & Concepts

- **Language**: C++11/14
- **Data Structures**: STL containers (vector, map, set), AST nodes, linked lists
- **Algorithms**: 
  - Recursive descent parsing
  - FIRST/FOLLOW set computation
  - Data flow analysis (reaching definitions)
  - Expression tree evaluation
- **Design Patterns**: 
  - Visitor pattern for AST traversal
  - Strategy pattern for different semantic checks

## Building and Running

### Compilation
```bash
# Project 1
g++ -std=c++11 parser.cc lexer.cc inputbuf.cc -o poly_compiler

# Project 2
g++ -std=c++11 project2.cc lexer.cc inputbuf.cc -o grammar_analyzer

# Project 3
g++ -std=c++11 demo.cc execute.cc lexer.cc inputbuf.cc -o ir_compiler
```

### Execution
```bash
# Run with input file
./poly_compiler < test_input.txt
./grammar_analyzer < grammar_spec.txt
./ir_compiler < program.txt
```

## Input Formats

### Project 1: Polynomial Language
```
TASKS 1 2 3 4;
POLY
    quadratic(a, b, c) = a*x^2 + b*x + c;
    linear(m) = m*x + 5;
EXECUTE
    INPUT x;
    y = quadratic(1, 2, 3);
    OUTPUT y;
INPUTS 10;
```

### Project 2: Grammar Specification
```
S -> A B *
A -> a | * 
B -> b *
#
```

### Project 3: IR Language
```
VAR x, y, sum;
{
    INPUT x;
    INPUT y;
    sum = x + y;
    OUTPUT sum;
}
INPUTS 5 10;
```

## Learning Outcomes

This project series demonstrates proficiency in:
- Compiler design theory and implementation
- Complex algorithm implementation (parsing, analysis)
- Memory management and pointer manipulation
- Object-oriented design principles
- STL and modern C++ features
- Software engineering best practices

## Error Handling

Each project includes robust error detection:
- **Syntax Errors**: Malformed input with line numbers
- **Semantic Errors**: Type mismatches, undeclared identifiers
- **Runtime Errors**: Division by zero, null pointer checks
- **Warnings**: Code quality issues (uninitialized vars, dead code)

## Performance Considerations

- Efficient symbol table lookups using hash maps
- Linear-time parsing with LL(1) grammar
- Optimized memory allocation strategy
- Minimal backtracking in parsing

## Author

**Krishna Balaji**  
ASU ID: 1226254343  
Course: CSE 340 - Principles of Programming Languages

## License

See LICENSE file for details.

## Acknowledgments

Based on course materials and framework provided by:
- Dr. Rida Bazzi (2017-2025)
- Dr. Raha Moraffah (2018)

---

*Note: This is an academic project demonstrating compiler construction principles. The implementations follow standard compiler design patterns and algorithms taught in undergraduate compiler courses.*
