/* blazer.c - A scanner and parser for the Blazer language
 * Context-Free Grammar:
 * <program> -> <stmt>{<stmt>}'halt'
 * <stmt>    -> T_ID '=' <expr> | 'read' T_ID | 'write' <expr>
 * <expr>    -> <term> {('+' | '-') <term>}
 * <term>    -> <factor> {('*' | '/') <factor>}
 * <factor>  -> T_ID | T_INT_CONSTANT | '(' <expr> ')'
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Global declarations */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp;
int errorCount = 0;
int haltFound = 0;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void program();
void statement();
void expr();
void term();
void factor();
void error(const char* msg);

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define READ_KW 27
#define WRITE_KW 28
#define HALT_KW 29

/******************************************************/
/* main driver */
int main(int argc, char *argv[]) {
    char filename[100];
    
    /* Determine input file */
    if (argc > 1) {
        strcpy(filename, argv[1]);
    } else {
        strcpy(filename, "front.in");
    }
    
    /* Open the input data file */
    if ((in_fp = fopen(filename, "r")) == NULL) {
        printf("ERROR - cannot open %s\n", filename);
        exit(1);
    }
    
    printf("\nstarting the parsing process...\n");
    
    /* Initialize lexer */
    getChar();
    lex();
    
    /* Start parsing */
    program();
    
    /* Close file */
    fclose(in_fp);
    
    /* Report results */
    if (errorCount > 0) {
        printf("\n--- Found %d error(s) ---\n", errorCount);
        printf("This program is invalid.\n");
    } else {
        printf("This is a valid program.\n");
    }
    
    return 0;
}

/*****************************************************/
/* error - handle parsing errors */
void error(const char* msg) {
    errorCount++;
    printf("%d: %s\n", errorCount, msg);
}

/*****************************************************/
/* lookup - lookup operators and parentheses */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        default:
            addChar();
            nextToken = EOF;
            break;
    }
    return nextToken;
}

/*****************************************************/
/* addChar - add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else {
        printf("Error - lexeme is too long\n");
    }
}

/*****************************************************/
/* getChar - get next character and determine its class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    } else {
        charClass = EOF;
    }
}

/*****************************************************/
/* getNonBlank - skip whitespace */
void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

/*****************************************************/
/* lex - lexical analyzer */
int lex() {
    lexLen = 0;
    getNonBlank();
    
    switch (charClass) {
        /* Identifiers and Keywords */
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            
            /* Check for keywords */
            if (strcmp(lexeme, "read") == 0)
                nextToken = READ_KW;
            else if (strcmp(lexeme, "write") == 0)
                nextToken = WRITE_KW;
            else if (strcmp(lexeme, "halt") == 0)
                nextToken = HALT_KW;
            else
                nextToken = IDENT;
            break;
            
        /* Integer literals */
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;
            
        /* Operators and parentheses */
        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;
            
        /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    }
    
    printf("Call lex...%s\n", lexeme);
    return nextToken;
}

/*****************************************************/
/* program - parse <program> -> <stmt>{<stmt>}'halt' */
void program() {
    printf("Enter <program>\n");
    
    /* Parse statements until 'halt' or EOF */
    while (nextToken != HALT_KW && nextToken != EOF) {
        statement();
    }
    
    /* Check for 'halt' keyword */
    if (nextToken == HALT_KW) {
        haltFound = 1;
        lex();
        
        /* Check for statements after 'halt' */
        if (nextToken != EOF) {
            error("ERROR: STATEMENTS AFTER 'halt'.");
        }
    } else {
        error("ERROR: NO 'halt' STATEMENT.");
    }
    
    printf("Exit <program>\n");
}

/*****************************************************/
/* statement - parse <stmt> */
void statement() {
    printf("Enter <statement>\n");
    
    if (nextToken == READ_KW) {
        /* 'read' T_ID */
        lex();
        if (nextToken == IDENT) {
            lex();
        } else {
            error("Invalid read statement.");
        }
    }
    else if (nextToken == WRITE_KW) {
        /* 'write' <expr> */
        lex();
        expr();
    }
    else if (nextToken == IDENT) {
        /* T_ID '=' <expr> */
        lex();
        if (nextToken == ASSIGN_OP) {
            lex();
            expr();
        } else {
            error("Invalid assignment statement.");
        }
    }
    else {
        /* Unexpected token */
        error("Invalid statement.");
        lex();  /* Skip the invalid token */
    }
    
    printf("Exit <statement>\n");
}

/*****************************************************/
/* expr - parse <expr> -> <term> {('+' | '-') <term>} */
void expr() {
    printf("Enter <expr>\n");
    
    term();
    
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
    
    printf("Exit <expr>\n");
}

/*****************************************************/
/* term - parse <term> -> <factor> {('*' | '/') <factor>} */
void term() {
    printf("Enter <term>\n");
    
    factor();
    
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
    }
    
    printf("Exit <term>\n");
}

/*****************************************************/
/* factor - parse <factor> -> T_ID | T_INT_CONSTANT | '(' <expr> ')' */
void factor() {
    printf("Enter <factor>\n");
    
    if (nextToken == IDENT || nextToken == INT_LIT) {
        /* T_ID or T_INT_CONSTANT */
        lex();
    }
    else if (nextToken == LEFT_PAREN) {
        /* '(' <expr> ')' */
        lex();
        expr();
        if (nextToken == RIGHT_PAREN) {
            lex();
        } else {
            error("Missing closing parenthesis in factor.");
        }
    }
    else {
        /* Invalid token in factor */
        error("Invalid token in factor.");
        lex();
    }
    
    printf("Exit <factor>\n");
}