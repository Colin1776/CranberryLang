#ifndef PARSER_H
#define PARSER_H

#include "types.h"

enum Operator
{
    OP_ACCESS,
    OP_ASSIGN,

    OP_ADD_ASSIGN,
    OP_SUBTRACT_ASSIGN,
    OP_MULTIPLY_ASSIGN,
    OP_DIVIDE_ASSIGN,
	OP_MODULO_ASSIGN,

	OP_INCREMENT,
	OP_DECREMENT,

    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
	OP_MODULO,
	
    OP_EQUALITY,
	OP_INEQUALITY,
    OP_GREATER_EQUALS,
    OP_LESS_EQUALS,
	OP_GREATER_THAN,
	OP_LESS_THAN,

	OP_LOGICAL_AND,
	OP_LOGICAL_OR,
	OP_LOGICAL_NOT,

	OP_ADDRESS,
	OP_POINTER,
	OP_DEREF
};

enum ASTType
{
    AST_IDENTIIFER,

    AST_IMPORT,
    AST_FUNC_DEF,
    AST_FUNC_CALL,
    AST_VAR_DEF,
    AST_ARRAY_TYPE,
    AST_BINARY_OP,
	AST_UNARY_OP,
    AST_LITERAL,
    AST_RETURN,
    AST_IF,
    AST_BLOCK,
    AST_LOOP
};

struct ASTNode
{
    enum ASTType type;
    union
    {
        char* string;
        struct ASTFuncDef* func_def;
    	struct ASTFuncCall* func_call;
    	struct ASTVarDef* var_def;
    	struct ASTArrayType* arr_type;
    	struct ASTBinaryOp* binary_op;
		struct ASTUnaryOp* unary_op;
    	struct ASTLiteral* literal;
    	struct ASTNode* return_expression;
    	struct ASTIfStatement* if_statement;
		struct ASTRoot* block;
		struct ASTLoop* loop;
    };
    
};

struct ASTRoot
{
    unsigned long number_of_nodes;
    unsigned long max_nodes;
    struct ASTNode* nodes;
};

struct ASTArrayType
{
    struct ASTNode* type;
};

struct ASTFuncDef
{
    struct ASTNode* return_type;
    char* func_name;
    struct ASTRoot* args;
    struct ASTRoot* block;
};

struct ASTFuncCall
{
    char* func_name;
    struct ASTRoot* args;
};

struct ASTVarDef
{
    struct ASTNode* type;
	  char* ident;
};

struct ASTBinaryOp
{
    enum Operator operator;
	struct ASTNode* left;
	struct ASTNode* right;
};

struct ASTUnaryOp
{
	enum Operator operator;
	struct ASTNode* operand;
};

struct ASTIfStatement
{
    struct ASTNode* condition;
    struct ASTRoot* block;
    struct ASTNode* else_block;
};


enum LiteralType
{
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_CHAR,
    LITERAL_STRING
};

struct ASTLiteral
{
    enum LiteralType type;
    union 
    {
        char* string;
        double floating;
        unsigned long integer;
    };
};

struct ASTLoop
{
    struct ASTNode* block;
    struct ASTNode* condition;
};

struct Parser
{
    unsigned long number_of_tokens;
    struct Token* token_array;
    unsigned long current_token;
    boolean mod_scope;
	boolean class_scope;
	boolean func_scope;
};

void parser_error(struct Parser, struct ASTNode, struct Token, char* message);
struct ASTRoot* parse_block(struct Parser*);
struct ASTNode* parse_expression(struct Parser*, unsigned long);
struct ASTNode* parse_statement(struct Parser*, unsigned int);
void print_AST_node(struct ASTNode, unsigned int, unsigned int);
struct ASTRoot* parse(struct Parser*);
void print_AST(struct ASTRoot*);

#endif
