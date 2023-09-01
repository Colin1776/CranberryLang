#ifndef LEXER_H
#define LEXER_H

enum TokenType
{
    TOKEN_IDENTIFIER = 256,

    TOKEN_KEYWORD_BREAK,
    TOKEN_KEYWORD_CASE,
    TOKEN_KEYWORD_CLASS,
    TOKEN_KEYWORD_CONST,
    TOKEN_KEYWORD_DEFAULT,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_IMPORT,
    TOKEN_KEYWORD_IN,
    TOKEN_KEYWORD_LONG,
    TOKEN_KEYWORD_LOOP,
    TOKEN_KEYWORD_MAIN,
    TOKEN_KEYWORD_NEW,
    TOKEN_KEYWORD_RETURN,
    TOKEN_KEYWORD_STRUCT,
    TOKEN_KEYWORD_SWITCH,
    TOKEN_KEYWORD_VOID,
    
    TOKEN_LITERAL_INTEGER,
    TOKEN_LITERAL_CHARACTER,
    TOKEN_LITERAL_FLOATING,
    TOKEN_LITERAL_STRING,

    TOKEN_ADD_ASSIGN,
    TOKEN_SUBTRACT_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,
    TOKEN_LEFT_SHIFT_ASSIGN,
    TOKEN_RIGHT_SHIFT_ASSIGN,
    TOKEN_CONJUNCT_ASSIGN,
    TOKEN_INCLUSIVE_DISTJUNCT_ASSIGN,
    TOKEN_EXCLUSIVE_DISJUNCT_ASSIGN,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_EQUALITY,
    TOKEN_INEQUALITY,
    TOKEN_LESS_EQUALS,
    TOKEN_GREATER_EQUALS,
    TOKEN_LOGICAL_AND,
    TOKEN_LOGICAL_OR,
    TOKEN_LEFT_SHIFT,
    TOKEN_RIGHT_SHIFT,
};

struct Token
{
    enum TokenType type;
    unsigned int char_start;
    unsigned int char_end;
	unsigned long line;
    union 
    {
        unsigned long integer;
        double decimal;
        char* string;
    };
};

struct Lexer
{
    unsigned long number_of_tokens;
    unsigned long max_tokens;
    struct Token* token_array;
    char* file_data;
	char* file_path;
    unsigned long file_size;
    unsigned long current_character;
    unsigned long current_line;
};

/* Function Declarations :) */

void lexer_error(struct Lexer*, char* msg);

int is_number(char);
int is_letter(char);
int is_alpha_numeric(char);
int is_spacer(char);

int get_keyword(char*, int);

void handle_word(struct Token*, struct Lexer*);
void handle_number(struct Token*, struct Lexer*);
void handle_character(struct Token*, struct Lexer*);
void handle_string(struct Token*, struct Lexer*);
void handle_plus(struct Token*, struct Lexer*);
void handle_minus(struct Token*, struct Lexer*);
void handle_asterisk(struct Token*, struct Lexer*);
void handle_slash(struct Token*, struct Lexer*);
void handle_mod(struct Token*, struct Lexer*);
void handle_equals(struct Token*, struct Lexer*);
void handle_left_angle(struct Token*, struct Lexer*);
void handle_right_angle(struct Token*, struct Lexer*);
void handle_ampersand(struct Token*, struct Lexer*);
void handle_bar(struct Token*, struct Lexer*);
void handle_carrot(struct Token*, struct Lexer*);
void handle_exclamation(struct Token*, struct Lexer*);

void add_token(struct Lexer*);
void lex(struct Lexer*);


#endif
