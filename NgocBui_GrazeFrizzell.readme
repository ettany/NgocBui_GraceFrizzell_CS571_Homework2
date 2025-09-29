# Blazer Language Scanner and Parser
## Homework 2 Implementation

**Student Name(s):** Ngoc Bui and Grace Frizzell

---

## Building and Executing the Program

### Compilation
To compile the program, use the following command:
```bash
gcc -o blazer NgocBui_GraceFrizzell_blazer.c
```

Or with warnings enabled (recommended):
```bash
gcc -Wall -o blazer NgocBui_GraceFrizzell_blazer.c
```

### Execution
To run the program on a Blazer source file:
```bash
./blazer sample0.bl
./blazer sample1.bl
./blazer sample2.bl
```

The program expects a single command-line argument: the path to the `.bl` source file.

---

## Design Overview

### Scanner (Lexer)
The scanner implements a simple character-by-character tokenization approach:
- **get_char()**: Reads one character at a time from the source file
- **skip_whitespace()**: Skips over whitespace characters
- **get_next_token()**: Main tokenization function that recognizes:
  - Keywords: `read`, `write`, `halt`
  - Identifiers: letter followed by letters/digits
  - Integer constants: sequences of digits
  - Operators: `=`, `+`, `-`, `*`, `/`
  - Delimiters: `(`, `)`

Keywords are recognized using `strcmp()` after an identifier is scanned.

### Parser
The parser uses recursive descent parsing with one function per non-terminal:
- **program()**: Parses the overall program structure
- **stmt()**: Handles assignment, read, and write statements
- **expr()**: Parses expressions with addition/subtraction
- **term()**: Parses terms with multiplication/division
- **factor()**: Parses factors (identifiers, constants, parenthesized expressions)

The parser follows operator precedence naturally through the grammar structure.

---

## Difficulties Encountered

1. **Initial Character Reading**: Setting up the initial character reading properly before starting tokenization required careful attention to ensure the first token was read correctly.

2. **Keyword Recognition**: Implementing the distinction between identifiers and keywords required using `strcmp()` after scanning an identifier token.

3. **Error Handling**: Providing context-sensitive error messages with line numbers and current token information required passing state through the parsing functions.

4. **Whitespace Handling**: Ensuring whitespace and newlines were properly handled while maintaining accurate line number tracking.

---

## Reflections and Comments

This assignment provided valuable hands-on experience in:
- Understanding the relationship between regular expressions and tokenization
- Implementing recursive descent parsing based on a context-free grammar
- The importance of proper error handling and meaningful error messages
- How compiler front-ends work at a fundamental level

The modular design (separate functions for each grammar rule) makes the code maintainable and easy to extend. Future enhancements could include:
- Symbol table implementation for semantic analysis
- Better error recovery mechanisms
- Support for comments in the source language
- Code generation to produce executable output

---

## Test Files Description

- **sample0.bl**: Simple test with assignment and write statement
- **sample1.bl**: Tests read statements and multiple operations
- **sample2.bl**: Complex expressions with parentheses and operator precedence