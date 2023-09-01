#include "include/lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Function Definitions :) */

void lex(struct Lexer* lexer)
{
    while (lexer->current_character < lexer->file_size)
    {
        if (lexer->number_of_tokens == lexer->max_tokens)
        {
            lexer->max_tokens += 100;
            lexer->token_array = realloc(lexer->token_array, sizeof(struct Token) * lexer->max_tokens);
        }

        add_token(lexer);
    }
}

void add_token(struct Lexer* lexer)
{
    char character = lexer->file_data[lexer->current_character];

    if (is_spacer(character))
    {
        lexer->current_character++;

        if (character == '\n')
        {
            lexer->current_line++;
        }

        return;
    }

    struct Token* token = malloc(sizeof(struct Token));

    token->type = -1;
    token->char_start = lexer->current_character;
	
	switch(character)
	{
		case 65 ... 90:
		case 97 ... 122:
			handle_word(token, lexer);
			break;
		case 48 ... 57:
			handle_number(token, lexer);
			break;
		case '\'':
			handle_character(token, lexer);
			break;
		case '"':
			handle_string(token, lexer);
			break;
		case '+':
			handle_plus(token, lexer);
			break;
		case '-':
			handle_minus(token, lexer);
			break;
		case '*':
			handle_asterisk(token, lexer);
			break;
		case '/':
			handle_slash(token, lexer);
			break;
		case '%':
			handle_mod(token, lexer);
			break;
		case '=':
			handle_equals(token, lexer);
			break;
		case ',':
			token->type = ',';
			lexer->current_character++;
			break;
		case '.':
			token->type = '.';
			lexer->current_character++;
			break;
		case ':':
			token->type = ':';
			lexer->current_character++;
			break;
		case ';':
			token->type = ';';
			lexer->current_character++;
			break;
		case '[':
			token->type = '[';
			lexer->current_character++;
			break;
		case ']':
			token->type = ']';
			lexer->current_character++;
			break;
		case '{':
			token->type = '{';
			lexer->current_character++;
			break;
		case '}':
			token->type = '}';
			lexer->current_character++;
			break;
		case '<':
			handle_left_angle(token, lexer);
			break;
		case '>':
			handle_right_angle(token, lexer);
			break;
		case '(':
			token->type = '(';
			lexer->current_character++;
			break;
		case ')':
			token->type = ')';
			lexer->current_character++;
			break;
		case '&':
			handle_ampersand(token, lexer);
			break;
		case '|':
			handle_bar(token, lexer);
			break;
		case '^':
			handle_carrot(token, lexer);
			break;
		case '!':
			handle_exclamation(token, lexer);
			break;
		default:
			lexer_error(lexer, "Invalid Character");
	}
	
	token->line = lexer->current_line;
    lexer->token_array[lexer->number_of_tokens] = *token;
    lexer->number_of_tokens++;
}

void handle_word(struct Token* token, struct Lexer* lexer)
{
    char character;

    while (lexer->current_character < lexer->file_size)
    {
        lexer->current_character++;
        character = lexer->file_data[lexer->current_character];

        if (!is_alpha_numeric(character))
        {
            token->char_end = lexer->current_character;
            break;        
        }
    }

    int size_of_ident = token->char_end - token->char_start;
    char* ident_string = malloc(sizeof(char) * size_of_ident + 1);

    for (int i = 0; i < size_of_ident; i++)
    {
        ident_string[i] = lexer->file_data[token->char_start + i];
    }
    ident_string[size_of_ident] = '\0';

    int keyword = get_keyword(ident_string, size_of_ident);
    token->type = keyword;
    token->string = ident_string;
}

// THIS IS INCOMPLETE AND NEEDS TO BE FIXED AND REWRITTEN PLEASE
// I GOTTA CHECK THE SIGN AND THE SIZE OR SMTN
void handle_number(struct Token* token, struct Lexer* lexer)
{
    char character;

    while (lexer->current_character < lexer->file_size)
    {
        lexer->current_character++;
        character = lexer->file_data[lexer->current_character];

        if (!is_number(character))
        {
            token->char_end = lexer->current_character;
            break;
        }
    }

    int size_of_num = token->char_end - token->char_start;
    char* num_string = malloc(sizeof(char) * size_of_num);

    for (int i = 0; i < size_of_num; i++)
    {
        num_string[i] = lexer->file_data[token->char_start + i];
    }

    unsigned int num = atoll(num_string);
    token->type = TOKEN_LITERAL_INTEGER;
    token->integer = num;
}

void handle_character(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '\'')
    {
        lexer_error(lexer, "Expected a character for char literal");
    }
    else if (character == '\\')
    {
        // handle logic here for stuff 
    }
    else
    {
        lexer->current_character++;
        char next = lexer->file_data[lexer->current_character];

        if (next != '\'')
        {
            lexer_error(lexer, "Too many characters in char literal");
        }
        else
        {
            lexer->current_character++;
            token->type = TOKEN_LITERAL_CHARACTER;
            token->integer = character;
            token->char_end = lexer->current_character;
        }
    }

}

void handle_string(struct Token* token, struct Lexer* lexer)
{
    char character;

    token->char_start++;
    while (lexer->current_character < lexer->file_size)
    {
        lexer->current_character++;
        character = lexer->file_data[lexer -> current_character];

        if (character == '"')
        {
            token->char_end = lexer->current_character;
            break;   
        }
    }

    int string_size = token->char_end - token->char_start;
    char* string = malloc(string_size + 1);

    for (int i = 0; i < string_size; i++)
    {
        string[i] = lexer->file_data[token->char_start + i];
    }

    string[string_size] = '\0';

    token->type = TOKEN_LITERAL_STRING;
    token->string = string;
    lexer->current_character++;
}

void handle_plus(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '+')
    {
        token->type = TOKEN_INCREMENT;
        lexer->current_character++;
    }
    else if (character == '=')
    {
        token->type = TOKEN_ADD_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '+';
    }
}

void handle_minus(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '-')
    {
        token->type = TOKEN_DECREMENT;
        lexer->current_character++;
    }
    else if (character == '=')
    {
        token->type = TOKEN_SUBTRACT_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '-';
    }
}

void handle_asterisk(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '=')
    {
        token->type = TOKEN_MULTIPLY_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '*';
    }
}

void handle_slash(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];
    
    if (character == '/')
    {
        // le comment
        // after line system implement le comments
    }
    else if (character == '=')
    {
        token->type = TOKEN_DIVIDE_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '/';
    }
}

void handle_mod(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '=')
    {
        token->type = TOKEN_MODULO_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '%';
    }
}

void handle_equals(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '=')
    {
        token->type = TOKEN_EQUALITY;
        lexer->current_character++;
    }
    else
    {
        token->type = '=';
    }
}

void handle_left_angle(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '<')
    {
        lexer->current_character++;
        character = lexer->file_data[lexer->current_character];

        if (character == '=')
        {
            token->type = TOKEN_LEFT_SHIFT_ASSIGN;
            lexer->current_character++;
        }
        else
        {
            token->type = TOKEN_LEFT_SHIFT;
        }
    }
    else if (character == '=')
    {
        token->type = TOKEN_LESS_EQUALS;
        lexer->current_character++;
    }
    else
    {
        token->type = '<';
    }
}

void handle_right_angle(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '>')
    {
        lexer->current_character++;
        character = lexer->file_data[lexer->current_character];

        if (character == '=')
        {
            token->type = TOKEN_RIGHT_SHIFT_ASSIGN;
            lexer->current_character++;
        }
        else
        {
            token->type = TOKEN_RIGHT_SHIFT;
        }
    }
    else if (character == '=')
    {
        token->type = TOKEN_GREATER_EQUALS;
        lexer->current_character++;
    }
    else
    {
        token->type = '>';
    }
}

void handle_ampersand(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '=')
    {
        token->type = TOKEN_EXCLUSIVE_DISJUNCT_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '&';
    }
}

void handle_bar(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '|')
    {
        token->type = TOKEN_LOGICAL_OR;
        lexer->current_character++;
    }
    else if (character == '=')
    {
        token->type = TOKEN_INCLUSIVE_DISTJUNCT_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '|';
    }
}

void handle_carrot(struct Token* token, struct Lexer* lexer)
{
    lexer->current_character++;
    char character = lexer->file_data[lexer->current_character];

    if (character == '=')
    {
        token->type = TOKEN_EXCLUSIVE_DISJUNCT_ASSIGN;
        lexer->current_character++;
    }
    else
    {
        token->type = '^';
    }
}

void handle_exclamation(struct Token* token, struct Lexer* lexer)
{
	lexer->current_character++;
	char character = lexer->file_data[lexer->current_character];
	
	if (character == '=')
	{
		token->type = TOKEN_INEQUALITY;
		lexer->current_character++;
	}
	else
	{
		token->type = '!';
	}
}

int get_keyword(char* string, int len)
{
    switch (len)
    {
        case 2:
            if (!memcmp(string, "if", 2)) 
                return TOKEN_KEYWORD_IF;
            if (!memcmp(string, "in", 2))
                return TOKEN_KEYWORD_IN;
            break;
            
        case 3:
            if (!memcmp(string, "new", 3))
                return TOKEN_KEYWORD_NEW;
            break;

        case 4:
            if (!memcmp(string, "case", 4))
                return TOKEN_KEYWORD_CASE;
            if (!memcmp(string, "else", 4))
                return TOKEN_KEYWORD_ELSE;
            if (!memcmp(string, "long", 4))
                return TOKEN_KEYWORD_LONG;
            if (!memcmp(string, "loop", 4))
                return TOKEN_KEYWORD_LOOP;
            if (!memcmp(string, "main", 4))
                return TOKEN_KEYWORD_MAIN;
            if (!memcmp(string, "void", 4))
                return TOKEN_KEYWORD_VOID;
            break;
            
        case 5:
            if (!memcmp(string, "break", 5))
                return TOKEN_KEYWORD_BREAK;
            if (!memcmp(string, "class", 5))
                return TOKEN_KEYWORD_CLASS;
            if (!memcmp(string, "const", 5))
                return TOKEN_KEYWORD_CONST;
            break;
            
        case 6:
            if (!memcmp(string, "import", 6))
                return TOKEN_KEYWORD_IMPORT;
            if (!memcmp(string, "return", 6))
                return TOKEN_KEYWORD_RETURN;
            if (!memcmp(string, "switch", 6))
                return TOKEN_KEYWORD_SWITCH;
            if (!memcmp(string, "struct", 6))
                return TOKEN_KEYWORD_STRUCT;
            break;
        
        case 7:
            if (!memcmp(string, "default", 7))
                return TOKEN_KEYWORD_DEFAULT;
            break;
    }

    return TOKEN_IDENTIFIER;
}

int is_number(char character)
{
    return character >= 48 && character <= 57;
}

int is_letter(char character)
{
    return (character >= 65 && character <= 90) || (character == 95) || (character >= 97 && character <= 122);
}

int is_alpha_numeric(char character)
{
    return is_number(character) || is_letter(character);
}

int is_spacer(char character)
{
    return character == 10 || character == 13 || character == 9 || character == 32;
}

void lexer_error(struct Lexer* lexer, char* msg)
{
	unsigned long line = lexer->current_line;
	
	printf("Lexical error at line %lu in %s:\n\t%s\n", line, lexer->file_path, msg);
	
	exit(-1);
}

void print_token(struct Token token)
{
    if (token.type < 256)
    {
        printf("Symbol: %c\n", token.type);
        return;
    }

    switch (token.type)
    {
        // printing identifier
        case TOKEN_IDENTIFIER:
            printf("Identifier: %s\n", token.string);
            break;
        
        // printing keywords
        case TOKEN_KEYWORD_BREAK:
            printf("Keyword: break\n");
            break;
        case TOKEN_KEYWORD_CASE:
            printf("Keyword: case\n");
            break;
        case TOKEN_KEYWORD_CLASS:
            printf("Keyword: class\n");
            break;
        case TOKEN_KEYWORD_CONST:
            printf("Keyword: const\n");
            break;
        case TOKEN_KEYWORD_DEFAULT:
            printf("Keyword: default\n");
            break;
        case TOKEN_KEYWORD_ELSE:
            printf("Keyword: else\n");
            break;
        case TOKEN_KEYWORD_IF:
            printf("Keyword: if\n");
            break;
        case TOKEN_KEYWORD_IMPORT:
            printf("Keyword: import\n");
            break;
        case TOKEN_KEYWORD_LONG:
            printf("Keyword: long\n");
            break;
        case TOKEN_KEYWORD_LOOP:
            printf("Keyword: loop\n");
            break;
        case TOKEN_KEYWORD_MAIN:
            printf("Keyword: main\n");
            break;
        case TOKEN_KEYWORD_NEW:
            printf("Keyword: new\n");
            break;
        case TOKEN_KEYWORD_RETURN:
            printf("Keyword: return\n");
            break;
        case TOKEN_KEYWORD_STRUCT:
            printf("Keyword: struct\n");
            break;
        case TOKEN_KEYWORD_SWITCH:
            printf("Keyword: swtich\n");
            break;
        case TOKEN_KEYWORD_VOID:
            printf("Keyword: void\n");
            break;

        // printing literals
        case TOKEN_LITERAL_CHARACTER:
            printf("Character Literal: %lu\n", token.integer);
            break;
        case TOKEN_LITERAL_INTEGER:
            printf("Integer Literal: %lu\n", token.integer);
            break;
        case TOKEN_LITERAL_STRING:
            printf("String Literal: %s\n", token.string);
            break;

        // printing symbols
        case TOKEN_ADD_ASSIGN:
            printf("Symbol: +=\n");
            break;
        case TOKEN_SUBTRACT_ASSIGN:
            printf("Symbol: -=\n");
            break;
        case TOKEN_MULTIPLY_ASSIGN:
            printf("Symbol: *=\n");
            break;
        case TOKEN_DIVIDE_ASSIGN:
            printf("Symbol: /=\n");
            break;
        case TOKEN_MODULO_ASSIGN:
            printf("Symbol: %%=\n");
            break;
        case TOKEN_LEFT_SHIFT_ASSIGN:
            printf("Symbol: <<=\n");
            break;
        case TOKEN_RIGHT_SHIFT_ASSIGN:
            printf("Symbol: >>=\n");
            break;
        case TOKEN_CONJUNCT_ASSIGN:
            printf("Symbol: &=\n");
            break;
        case TOKEN_INCLUSIVE_DISTJUNCT_ASSIGN:
            printf("Symbol: |=\n");
            break;
        case TOKEN_EXCLUSIVE_DISJUNCT_ASSIGN:
            printf("Symbol: ^=\n");
            break;
        case TOKEN_INCREMENT:
            printf("Symbol: ++\n");
            break;
        case TOKEN_DECREMENT:
            printf("Symbol: --\n");
            break;
        case TOKEN_EQUALITY:
            printf("Symbol: ==\n");
            break;
        case TOKEN_INEQUALITY:
            printf("Symbol: !=\n");
            break;
        case TOKEN_LESS_EQUALS:
            printf("Symbol: <=\n");
            break;
        case TOKEN_GREATER_EQUALS:
            printf("Symbol: >=\n");
            break;
        case TOKEN_LOGICAL_AND:
            printf("Symbol: &&\n");
            break;
        case TOKEN_LOGICAL_OR:
            printf("Symbol: ||\n");
            break;
        case TOKEN_LEFT_SHIFT:
            printf("Symbol: <<\n");
            break;
        case TOKEN_RIGHT_SHIFT:
            printf("Symbol: >>\n");
            break;

        // printing default (in case i forgor a token lol)
        default:
            printf("ID: %d\n", token.type);
            break;
    }    
}

void print_lexer(struct Lexer* lexer)
{
    for (int i = 0; i < lexer->number_of_tokens; i++)
    {
        struct Token token = lexer->token_array[i];
        print_token(token);
    }
}