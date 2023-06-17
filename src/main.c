#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


#include "types.c"
#include "util.c"
#include "modulizer.c"
#include "parser.c"

int main(int argc, char** argv)
{
    char* file = NULL;
    u64 file_size = 0;

    for (int i = 0; i < argc; i++)
    {
        char* arg = argv[i];

        if (strcmp(arg, "main.cbry") == 0)
            file = load_file(arg, &file_size);
    }

    if (file == 0)
    {
        printf("No source file to transpile.\n");
        return -1;
    } 

    struct Lexer* lexer = malloc(sizeof(struct Lexer));
    lexer->number_of_tokens = 0;
    lexer->max_tokens = 100;
    lexer->token_array = malloc(sizeof(struct Token) * lexer->max_tokens);
    lexer->file_data = file;
	lexer->file_path = "main.cb";
    lexer->file_size = file_size;
    lexer->current_character = 0;
    lexer->current_line = 1;

    start_timer();

    lex(lexer);

    stop_timer("lexer");

    //print_lexer(lexer);

    struct Parser* parser = malloc(sizeof(struct Parser));
    parser->number_of_tokens = lexer->number_of_tokens;
    parser->token_array = lexer->token_array;
    parser->current_token = 0;
	parser->mod_scope = true;

    start_timer();

    struct ASTRoot* root = parse(parser);
    
    stop_timer("parser");

    print_AST(root);
    return 0;
}