/*
 * NgocBui_GraceFrizzell_blazer.c
 * Blazer Language Scanner and Parser
 * Implements lexical analysis and recursive descent parsing
 * for a simple language with arithmetic expressions and I/O
 * 
 * Authors: Ngoc Bui and Grace Frizzell
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Token types */
typedef enum {
    T_ID,
    T_INT_CONSTANT,
    T_ASSIGN,      // =
    T_PLUS,        // +
    T_MINUS,       // -
    T_TIMES,       // *
    T_DIV,         // /
    T_LPAREN,      // (
    T_RPAREN,      // )
    T_READ,        // read
    T_WRITE,       // write
    T_HALT,        // halt
    T_EOF,
    T_ERROR
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;
    char lexeme[256];
    int line_num;
} Token;

/* Global variables */
FILE *source_file;
Token current_token;
int line_number = 1;
char current_char;

/* Function prototypes */
void get_char();
void skip_whitespace();
Token get_next_token();
void match(TokenType expected);
void error(const char *message);

/* Parser function prototypes */
void program();
void stmt();
void expr();
void term();
void factor();

/* Scanner Implementation */

void get_char() {
    current_char = fgetc(source_file);
    if (current_char == '\n') {
        line_number++;
    }
}

void skip_whitespace() {
    while (isspace(current_char)) {
        get_char();
    }
}

Token get_next_token() {
    Token token;
    token.line_num = line_number;
    
    skip_whitespace();
    
    /* Check for EOF */
    if (current_char == EOF) {
        token.type = T_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    /* Check for identifiers and keywords */
    if (isalpha(current_char)) {
        int i = 0;
        while (isalnum(current_char) && i < 255) {
            token.lexeme[i++] = current_char;
            get_char();
        }
        token.lexeme[i] = '\0';
        
        /* Check for keywords */
        if (strcmp(token.lexeme, "read") == 0) {
            token.type = T_READ;
        } else if (strcmp(token.lexeme, "write") == 0) {
            token.type = T_WRITE;
        } else if (strcmp(token.lexeme, "halt") == 0) {
            token.type = T_HALT;
        } else {
            token.type = T_ID;
        }
        return token;
    }
    
    /* Check for integer constants */
    if (isdigit(current_char)) {
        int i = 0;
        while (isdigit(current_char) && i < 255) {
            token.lexeme[i++] = current_char;
            get_char();
        }
        token.lexeme[i] = '\0';
        token.type = T_INT_CONSTANT;
        return token;
    }
    
    /* Check for operators and delimiters */
    switch (current_char) {
        case '=':
            token.type = T_ASSIGN;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case '+':
            token.type = T_PLUS;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case '-':
            token.type = T_MINUS;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case '*':
            token.type = T_TIMES;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case '/':
            token.type = T_DIV;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case '(':
            token.type = T_LPAREN;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        case ')':
            token.type = T_RPAREN;
            token.lexeme[0] = current_char;
            token.lexeme[1] = '\0';
            get_char();
            break;
        default:
            token.type = T_ERROR;
            sprintf(token.lexeme, "Invalid character: '%c'", current_char);
            get_char();
            break;
    }
    
    return token;
}

/* Parser Implementation */

void error(const char *message) {
    fprintf(stderr, "Error at line %d: %s\n", current_token.line_num, message);
    fprintf(stderr, "Current token: %s\n", current_token.lexeme);
    exit(1);
}

void match(TokenType expected) {
    if (current_token.type == expected) {
        printf("Matched: %s\n", current_token.lexeme);
        current_token = get_next_token();
    } else {
        char error_msg[256];
        sprintf(error_msg, "Expected token type %d but found %s", 
                expected, current_token.lexeme);
        error(error_msg);
    }
}

/* <program> -> <stmt>{<stmt>}'halt' */
void program() {
    printf("Parsing program...\n");
    
    while (current_token.type != T_HALT && current_token.type != T_EOF) {
        stmt();
    }
    
    if (current_token.type == T_HALT) {
        match(T_HALT);
        printf("Program parsed successfully!\n");
    } else {
        error("Expected 'halt' at end of program");
    }
}

/* <stmt> -> T_ID '=' <expr>
          | 'read' T_ID
          | 'write' <expr> */
void stmt() {
    printf("Parsing statement...\n");
    
    if (current_token.type == T_ID) {
        match(T_ID);
        match(T_ASSIGN);
        expr();
    } else if (current_token.type == T_READ) {
        match(T_READ);
        match(T_ID);
    } else if (current_token.type == T_WRITE) {
        match(T_WRITE);
        expr();
    } else {
        error("Expected statement (assignment, read, or write)");
    }
}

/* <expr> -> <term> {('+'|'-')<term>} */
void expr() {
    printf("Parsing expression...\n");
    
    term();
    
    while (current_token.type == T_PLUS || current_token.type == T_MINUS) {
        if (current_token.type == T_PLUS) {
            match(T_PLUS);
        } else {
            match(T_MINUS);
        }
        term();
    }
}

/* <term> -> <factor> {('*'|'/') <factor>} */
void term() {
    printf("Parsing term...\n");
    
    factor();
    
    while (current_token.type == T_TIMES || current_token.type == T_DIV) {
        if (current_token.type == T_TIMES) {
            match(T_TIMES);
        } else {
            match(T_DIV);
        }
        factor();
    }
}

/* <factor> -> T_ID
            | T_INT_CONSTANT
            | '('<expr>')' */
void factor() {
    printf("Parsing factor...\n");
    
    if (current_token.type == T_ID) {
        match(T_ID);
    } else if (current_token.type == T_INT_CONSTANT) {
        match(T_INT_CONSTANT);
    } else if (current_token.type == T_LPAREN) {
        match(T_LPAREN);
        expr();
        match(T_RPAREN);
    } else {
        error("Expected identifier, constant, or '(' in factor");
    }
}

/* Main function */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file.bl>\n", argv[0]);
        return 1;
    }
    
    source_file = fopen(argv[1], "r");
    if (source_file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
        return 1;
    }
    
    printf("Blazer Language Parser\n");
    printf("======================\n");
    printf("Parsing file: %s\n\n", argv[1]);
    
    /* Initialize scanner */
    get_char();
    current_token = get_next_token();
    
    /* Start parsing */
    program();
    
    fclose(source_file);
    
    return 0;
}