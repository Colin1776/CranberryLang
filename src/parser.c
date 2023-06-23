#include "types.c"
#include "lexer.c"

enum Operator
{
	OP_ACCESS,
	OP_ASSIGN,

	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,

	OP_EQUALITY
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
    	struct ASTLiteral* literal;
    	struct ASTNode* return_expression;
    	struct ASTIfStatement* if_statement;
		struct ASTRoot* block;
		struct ASTLoop* loop;
    };
    
};

struct ASTRoot
{
    u64 number_of_nodes;
    u64 max_nodes;
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
        u64 integer;
    };
};

struct ASTLoop
{
	struct ASTNode* block;
};

struct Parser
{
    u64 number_of_tokens;
    struct Token* token_array;
    u64 current_token;
    bool mod_scope;
	bool class_scope;
	bool func_scope;
};

void parser_error(struct Parser, struct ASTNode, struct Token, char* message);
struct ASTRoot* parse_block(struct Parser*);
struct ASTNode* parse_expression(struct Parser*, u64);
struct ASTNode* parse_statement(struct Parser*, u32);
void print_AST_node(struct ASTNode, u8, u8);

struct Token* get_prev_token(struct Parser parser)
{
    if (parser.current_token != 0)
    {
        int prev = parser.current_token - 1;
        return &parser.token_array[prev];
    }

    return NULL;
}

struct Token* get_next_token(struct Parser parser)
{
	if (parser.current_token < parser.number_of_tokens - 1)
	{
		int next = parser.current_token + 1;
		return &parser.token_array[next];
	}
	else
	{
		printf("There should be more tokens here what the heck!!\n");
		exit(-1);
	}
	
	//return NULL;
}

u32 get_precedence(struct Token token)
{
	switch(token.type)
	{
		case ';':
		case ')':
		case ',':
			return 0;
		case TOKEN_EQUALITY:
			return 1;
		case '+':
		case '-':
			return 2;
		case '*':
		case '/':
			return 3;
		case '.':
			return 4;
		default:
			printf("Error: unknown precedence for token of type: %c on line: %d\n", token.type, token.line);
			exit(-1);
	}
}

void parse_import(struct Parser* parser, struct ASTNode* node)
{
	if (parser->mod_scope == false)
	{
		printf("Import statement must be at module scope\n");
		exit(-1);
	}
	
    struct Token current_token = parser->token_array[parser->current_token];
    parser->current_token++;
    
    struct Token next_token = parser->token_array[parser->current_token];

    if (next_token.type != TOKEN_IDENTIFIER)
    {
        printf("bad use of import at: \n");
        exit(-1);
    }

    node->type = AST_IMPORT;
    node->string = next_token.string;

    parser->current_token++;
    next_token = parser->token_array[parser->current_token];

    if (next_token.type != ';')
    {
        printf("import statement must end in semicolon\n");
        exit(-1);
    }

    parser->current_token++;
}

void parse_main(struct Parser* parser, struct ASTNode* node)
{
	if (parser->mod_scope == false)
	{
		printf("Main function must be defined at module scope\n");
		exit(-3);
	}
	
	parser->mod_scope = false;
	parser->func_scope = true;
	
    node->type = AST_FUNC_DEF;
    struct ASTFuncDef* func_def = malloc(sizeof(struct ASTFuncDef));
    struct ASTNode* return_type = malloc(sizeof(struct ASTNode));
    return_type->type = AST_IDENTIIFER;
    return_type->string = "int";
    func_def->return_type = return_type;
    func_def->func_name = "cranberryentrypoint";
    parser->current_token++;
    if (parser->token_array[parser->current_token].type != '{')
    {
        printf("Unexpected symbol after main\n");
        exit(-3);
    }
    parser->current_token++;
	
	struct ASTVarDef* var_def = malloc(sizeof(struct ASTVarDef));
	var_def->ident = "args";
	
	struct ASTNode* array_type_type = malloc(sizeof(struct ASTNode));
	array_type_type->type = AST_IDENTIIFER;
	array_type_type->string = "String";
	
	struct ASTArrayType* array_type = malloc(sizeof(struct ASTArrayType));
	array_type->type = array_type_type;
	
	struct ASTNode* var_def_type = malloc(sizeof(struct ASTNode));
	var_def_type->type = AST_ARRAY_TYPE;
	var_def_type->arr_type = array_type;
	
	var_def->type = var_def_type;
	
	struct ASTNode* param_node = malloc(sizeof(struct ASTNode));
	param_node->type = AST_VAR_DEF;
	param_node->var_def = var_def;
	
	struct ASTRoot* args = malloc(sizeof(struct ASTRoot));
	args->number_of_nodes = 1;
	args->max_nodes = 1;
	args->nodes = malloc(sizeof(struct ASTNode));
	
	args->nodes[0] = *param_node;
	
	func_def->args = args;
	
    func_def->block  = parse_block(parser);
    node->func_def = func_def;
	
	parser->mod_scope = true;
	parser->func_scope = false;
}

struct ASTRoot* parse_block(struct Parser* parser)
{
    struct ASTRoot* root = malloc(sizeof(struct ASTRoot));
    root->max_nodes = 100;
    root->number_of_nodes = 0;
    root->nodes = malloc(sizeof(struct ASTNode) * root->max_nodes);

    while (parser->current_token < parser->number_of_tokens)
    {
        if (root->number_of_nodes == root->max_nodes)
        {
            root->max_nodes += 100;
            root->nodes = realloc(root->nodes, sizeof(struct ASTNode) * root->max_nodes);
        }

        struct ASTNode* node = parse_statement(parser, 0);

        root->nodes[root->number_of_nodes] = *node;
        root->number_of_nodes++;
		
		if (parser->token_array[parser->current_token].type == '}')
        {
            parser->current_token++;
            break;
        }
    }

    return root;
}

struct ASTRoot* parse_args(struct Parser* parser)
{
    struct ASTRoot* root = malloc(sizeof(struct ASTRoot));
    root->max_nodes = 100;
    root->number_of_nodes = 0;
    root->nodes = malloc(sizeof(struct ASTNode) * root->max_nodes);

    while (parser ->current_token < parser->number_of_tokens)
    {
        if (root->number_of_nodes == root->max_nodes)
        {
            root->max_nodes += 100;
            root->nodes = realloc(root->nodes, sizeof(struct ASTNode) * root->max_nodes);
        }

        struct Token token = parser->token_array[parser->current_token];

        if (token.type == ')')
        {
            break;
        }

		struct ASTNode* arg = parse_expression(parser, 0);
		root->nodes[root->number_of_nodes] = *arg;
		root->number_of_nodes++;
    }
	
	return root;
}

struct ASTNode* operator(enum Operator op, struct ASTNode* left, struct ASTNode* right)
{
	struct ASTBinaryOp* bin_op = malloc(sizeof(struct ASTBinaryOp));
	bin_op->operator = op;
	bin_op->left = left;
	bin_op->right = right;

	struct ASTNode* node = malloc(sizeof(struct ASTNode));
	node->type = AST_BINARY_OP;
	node->binary_op = bin_op;
	return node;
}

struct ASTRoot* parse_params(struct Parser* parser)
{
	struct ASTRoot* params = malloc(sizeof(struct ASTRoot));
	params->max_nodes = 10;
	params->number_of_nodes = 0;

	params->nodes = malloc(sizeof(struct ASTNode) * params->max_nodes);

	struct ASTNode* node;

	while (1)
	{
		if (params->number_of_nodes == params->max_nodes)
        {
            params->max_nodes += 10;
            params->nodes = realloc(params->nodes, sizeof(struct ASTNode) * params->max_nodes);
        }

		struct Token token = parser->token_array[parser->current_token];

		if (token.type != TOKEN_IDENTIFIER)
			parser_error(*parser, *node, token, "Invalid token for parameter type in function definition");

		

		struct Token next = *get_next_token(*parser);

		if (next.type != TOKEN_IDENTIFIER)
			parser_error(*parser, *node, token, "Invalid token for parameter identifier in function definition");

		parser->current_token += 2;

		struct Token sep = parser->token_array[parser->current_token];

		if (sep.type == ',')
		{
			struct ASTVarDef* var_def = malloc(sizeof(struct ASTVarDef));
			var_def->type = malloc(sizeof(struct ASTNode));
			var_def->type->type = AST_IDENTIIFER;
			var_def->type->string = token.string;
			var_def->ident = next.string;

			node = malloc(sizeof(struct ASTNode));
			node->type = AST_VAR_DEF;
			node->var_def = var_def;

			params->nodes[params->number_of_nodes] = *node;
			params->number_of_nodes++;

			parser->current_token++;
		}
		else if (sep.type == ')')
		{
			struct ASTVarDef* var_def = malloc(sizeof(struct ASTVarDef));
			var_def->type = malloc(sizeof(struct ASTNode));
			var_def->type->type = AST_IDENTIIFER;
			var_def->type->string = token.string;
			var_def->ident = next.string;

			node = malloc(sizeof(struct ASTNode));
			node->type = AST_VAR_DEF;
			node->var_def = var_def;

			params->nodes[params->number_of_nodes] = *node;
			params->number_of_nodes++;

			parser->current_token++;
			break;
		}
		else
		{
			parser_error(*parser, *node, token, "Extra token after identifier in parameter, this bad get rid of :)");
			// error what the heck you should NOT have stuff here :cry:
		}

	}

	return params;
}

void parse_typed_definition(struct Parser* parser, struct ASTNode* node)
{
	struct ASTNode* type_node = malloc(sizeof(struct ASTNode));
	type_node->type = AST_IDENTIIFER;
	type_node->string = node->string;

	char* ident = parser->token_array[parser->current_token].string;

	struct Token token = *get_next_token(*parser); // this token should be the symbol after 2 identifiers or something

	if (token.type == '(')
	{
		if (parser->func_scope)
			parser_error(*parser, *node, token, "Defining a function is not allowed in the scope of a function definition");

		parser->func_scope = true;
		
		struct ASTFuncDef* func_def = malloc(sizeof(struct ASTFuncDef));
		func_def->return_type = type_node;
		func_def->func_name = ident;

		parser->current_token += 2; // token should now be at the thing after the '('
		func_def->args = parse_params(parser);

		token = parser->token_array[parser->current_token];

		if (token.type != '{')
			parser_error(*parser, *node, token, "Expected a '{' after if statement definition");
		
		parser->current_token++;

		func_def->block = parse_block(parser);

		node->type = AST_FUNC_DEF;
		node->func_def = func_def;

		parser->func_scope = false;

		// early exit
		return;
	}

	// if we get here it has to be a var def because reasons lol
	struct ASTVarDef* var_def = malloc(sizeof(struct ASTVarDef));
	var_def->type = type_node;
	var_def->ident = ident;

	if (token.type == ';')
	{
		node->type = AST_VAR_DEF;
		node->var_def = var_def;
		parser->current_token += 2;
	}
	else if (token.type == '=')
	{
		struct ASTBinaryOp* bin_op = malloc(sizeof(struct ASTBinaryOp));
		bin_op->operator = OP_ASSIGN;
		
		struct ASTNode* left_node = malloc(sizeof(struct ASTNode));
		left_node->type = AST_VAR_DEF;
		left_node->var_def = var_def;
		bin_op->left = left_node;
		
		parser->current_token += 2;
		bin_op->right = parse_expression(parser, 0);
		
		node->type = AST_BINARY_OP;
		node->binary_op = bin_op;
	}
	else
	{
		parser_error(*parser, *node, token, "Invalid symbol after variable definition");
	}
	
}

void parse_return(struct Parser* parser, struct ASTNode* node)
{
	//printf("among us!!!\n");
	
	if (!parser->func_scope)
		parser_error(*parser, *node, *(struct Token*)0, "Return has to be in function scope, idiot");

	node->type = AST_RETURN;
	parser->current_token++;
	node->return_expression = parse_expression(parser, 0);
}

/*
* Making this comment here so that I can remember exactly stuff
* Yeah, so where I commented out "parser->current_token++" is because of
* precedence stuff with the semicolon, I instead substituted it with
* "parser->current_token += 2" so that I only advance the tokens if I have
* precedence
*/
struct ASTNode* parse_expression(struct Parser* parser, u64 precedence)
{
	struct ASTNode* node = malloc(sizeof(struct ASTNode));
	node->type = -1;
	
	struct Token token = parser->token_array[parser->current_token];
	
	switch (token.type)
	{
		case TOKEN_IDENTIFIER:
			node->type = AST_IDENTIIFER;
			node->string = token.string;
			if (get_next_token(*parser)->type == '(')
			{
				struct ASTFuncCall* func_call = malloc(sizeof(struct ASTFuncCall));
				func_call->func_name = token.string;
				parser->current_token += 2;
				func_call->args = parse_args(parser);

				node->type = AST_FUNC_CALL;
				node->func_call = func_call;
			}
			break;

		case TOKEN_LITERAL_INTEGER:
			{struct ASTLiteral* literal = malloc(sizeof(struct ASTLiteral));
			literal->type = LITERAL_INT;
			literal->integer = token.integer;
			node->type = AST_LITERAL;
			node->literal = literal;}
			break;
		case TOKEN_LITERAL_STRING:
			{struct ASTLiteral* literal = malloc(sizeof(struct ASTLiteral));
			literal->type = LITERAL_STRING;
			literal->string = token.string;
			node->type = AST_LITERAL;
			node->literal = literal;}
			break;
		default:
			break;
	}

	struct Token* next = get_next_token(*parser);

	//parser->current_token++;

	u64 prec = get_precedence(*next);

	if (prec < precedence)
		return node;

	while (prec >= precedence)
	{
		switch (next->type)
		{
			case ')':
				parser->current_token += 1;
				return node;
			case ',':
				parser->current_token += 2;
				return node;
			case ';':
				parser->current_token += 2;
				return node;
			case '+':
				parser->current_token += 2;
				node = operator(OP_ADD, node, parse_expression(parser, prec + 1));
				break;
			case '-':
				parser->current_token += 2;
				node = operator(OP_SUBTRACT, node, parse_expression(parser, prec + 1));
				break;
			case '*':
				parser->current_token += 2;
				node = operator(OP_MULTIPLY, node, parse_expression(parser, prec + 1));
				break;
			case '/':
				parser->current_token += 2;
				node = operator(OP_DIVIDE, node, parse_expression(parser, prec + 1));
				break;
			case '.':
				parser->current_token += 2;
				node = operator(OP_ACCESS, node, parse_expression(parser, prec + 1));
				break;
			case TOKEN_EQUALITY:
				parser->current_token += 2;
				node = operator(OP_EQUALITY, node, parse_expression(parser, prec + 1));
				break;
		}

		next = get_next_token(*parser);
		prec = get_precedence(*next);
	}
		
	return node;
}

struct ASTRoot* from_node(struct ASTNode* node)
{
	struct ASTRoot* ret = malloc(sizeof(struct ASTRoot));
	ret->max_nodes = 1;
	ret->number_of_nodes = 1;
	ret->nodes = malloc(sizeof(struct ASTNode));
	ret->nodes[0] = *node;
	
	return ret;
}

void parse_if(struct Parser* parser, struct ASTNode* node)
{
	struct Token token = *get_next_token(*parser);

	if (!parser->func_scope)
		parser_error(*parser, *node, token, "if statements can't just be randomly stuff place yk");

	if (token.type != '(')
		parser_error(*parser, *node, token, "parenthesis expected\n");

	parser->current_token += 2;

	struct ASTIfStatement* if_stmnt = malloc(sizeof(struct ASTIfStatement));

	if_stmnt->condition = parse_expression(parser, 0);
	
	parser->current_token++;
	token = parser->token_array[parser->current_token];	

	if (token.type == '{')
	{
		parser->current_token++;
		if_stmnt->block = parse_block(parser);
	}
	else
	{
		if_stmnt->block = from_node(parse_statement(parser, 0));
	}

	token = parser->token_array[parser->current_token];

    if (token.type == TOKEN_KEYWORD_ELSE)
    {   
		parser->current_token++;
        if_stmnt->else_block = parse_statement(parser, 0);
    }

	node->type = AST_IF;
	node->if_statement = if_stmnt;
}

void parse_loop(struct Parser* parser, struct ASTNode* node)
{
	struct Token next = *get_next_token(*parser);

	// if (next.type != '{')
	// 	parser_error(*parser, *node, next, "Expected a block after loop keyword\n");

	struct ASTLoop* loop = malloc(sizeof(struct ASTLoop));
	struct ASTNode* block = malloc(sizeof(struct ASTNode));

	if (next.type == '{')
	{
		parser->current_token += 2;
		block->type = AST_BLOCK;
	
		block->block = parse_block(parser);
	}
	else
	{
		parser->current_token++;
		block = parse_statement(parser, 0);
	}

	loop->block = block;
	
	node->type = AST_LOOP;
	node->loop = loop;
}

struct ASTNode* parse_statement(struct Parser* parser, u32 current_precedence)
{
	//printf("%d / %d \n", parser->current_token, parser->number_of_tokens);
    struct ASTNode* node = malloc(sizeof(struct ASTNode));
    node->type = -1;

    struct Token token = parser->token_array[parser->current_token];

    switch (token.type)
    {
        case TOKEN_IDENTIFIER:
            node->type = AST_IDENTIIFER;
            node->string = token.string;
            //parser->current_token++;
            break;
        case TOKEN_KEYWORD_IMPORT:
            parse_import(parser, node);
            return node;
        case TOKEN_KEYWORD_MAIN:
            parse_main(parser, node);
            return node;
    	case TOKEN_KEYWORD_RETURN:
            parse_return(parser, node);
            return node;
		case TOKEN_KEYWORD_IF:
			parse_if(parser, node);
			return node;
        case '{':
            parser->current_token++;
			node->type = AST_BLOCK;
            node->block = parse_block(parser);
            return node;
		case TOKEN_KEYWORD_LOOP:
			parse_loop(parser, node);
			return node;
		default:
	    	parser_error(*parser, *node, token, "you're stupid lmao this token isn't supposed to be here goofy ahh");
        // default:
            // printf("Unexpected token\n");
			      // exit(-34);
    }
	
	struct Token* next = get_next_token(*parser);
	
	if (next == NULL)
		return node;
	
	if (next->type == TOKEN_IDENTIFIER)
	{
		parser->current_token++;
		parse_typed_definition(parser, node);
	}
	else if (next->type == '.')
	{
		node = parse_expression(parser, 0);	
	}
	
	

    return node;
}

struct ASTRoot* parse(struct Parser* parser)
{
    struct ASTRoot* root = malloc(sizeof(struct ASTRoot));
    root->number_of_nodes = 0;
    root->max_nodes = 100;
    root->nodes = malloc(sizeof(struct ASTNode) * root->max_nodes);

    while (parser->current_token < parser->number_of_tokens)
    {
        if (root->number_of_nodes == root->max_nodes)
        {
            root->max_nodes += 100;
            root->nodes = realloc(root->nodes, sizeof(struct ASTNode) * root->max_nodes);
        }

        

        struct ASTNode* node = parse_statement(parser, 0);
        root->nodes[root->number_of_nodes] = *node;
        root->number_of_nodes++;
    }

    return root;
}

void parser_error(struct Parser parser, struct ASTNode, struct Token token, char* error_msg)
{
	printf("%s\n", error_msg);
	printf("Token type: %d\n", token.type);
	exit(-1);
}

void print_tabs(u8 num_tabs)
{
	for (u8 i = 0; i < num_tabs; i++)
	{
		printf("\t");
	}
}

void print_AST_func_def(struct ASTNode node, u8 num_tabs)
{
	printf("Function Definition: \n");
	print_tabs(num_tabs + 1);
	printf("Return Type: (ident = int)\n");
	print_tabs(num_tabs + 1);
	printf("Identifier: %s\n", node.func_def->func_name);
	print_tabs(num_tabs + 1);
	printf("Parameters:\n");
	
	struct ASTRoot* args = node.func_def->args;
	
	for (u64 i = 0; i < args->number_of_nodes; i++)
	{
		struct ASTNode node = args->nodes[i];
		print_AST_node(node, 1, num_tabs + 2);
	}
	
	struct ASTRoot* block = node.func_def->block;
	
	print_tabs(num_tabs + 1);
	printf("Body:\n");
	
	for (u64 i = 0; i < block->number_of_nodes; i++)
	{
		struct ASTNode node = block->nodes[i];
		print_AST_node(node, 1, num_tabs + 2);
	}
}

void print_AST_var_def(struct ASTNode node, u8 num_tabs)
{
	struct ASTNode* type_node = node.var_def->type;
	
	char* ident_string = node.var_def->ident;
	
	printf("VarDef: (type = ");
	print_AST_node(*type_node, 0, 0);
	printf(", ident = \"%s\")\n", ident_string);
}

void print_AST_array_type(struct ASTNode node, u8 num_tabs)
{
	struct ASTArrayType* arr_type = node.arr_type;
	printf("Array (type = ");
	print_AST_node(*arr_type->type, 0, 0);
	printf(")");
}

void print_operator_type(enum Operator op)
{
	switch (op)
	{
		case OP_ACCESS:
			printf("access");
			break;
		case OP_ASSIGN:
			printf("assign");
			break;
		case OP_ADD:
			printf("add");
			break;
		case OP_SUBTRACT:
			printf("subtract");
			break;
		case OP_MULTIPLY:
			printf("multiply");
			break;
		case OP_DIVIDE:
			printf("divide");
			break;
		case OP_EQUALITY:
			printf("equality");
			break;
		default:
			printf("amongus");
			break;
	}
}

void print_binary_op(struct ASTNode node, u8 num_tabs)
{
	struct ASTBinaryOp bin_op = *node.binary_op;
	printf("Binary Operator\n");
	print_tabs(num_tabs + 1);
	printf("Operator: ");
	print_operator_type(bin_op.operator);
	printf("\n");

	struct ASTNode left = *bin_op.left;
	print_tabs(num_tabs + 1);
	printf("Left: \n");
	print_AST_node(left, 1, num_tabs + 2);

	struct ASTNode right = *bin_op.right;
	print_tabs(num_tabs + 1);
	printf("Right: \n");
	print_AST_node(right, 1, num_tabs + 2);
}

void print_AST_literal(struct ASTNode node)
{
	struct ASTLiteral lit = *node.literal;
	printf("Literal: (Type = ");

	switch (lit.type)
	{
		case LITERAL_INT:
			printf("Integer, Val = %llu)\n", lit.integer);
			break;
		case LITERAL_STRING:
			printf("String, Val = \"%s\")\n", lit.string);
			break;
	}
}

void print_AST_func_call(struct ASTNode node, u8 num_tabs)
{
	struct ASTFuncCall call = *node.func_call;

	printf("Function Call:\n");
	print_tabs(num_tabs + 1);
	printf("Identifier: %s\n",  call.func_name);
	print_tabs(num_tabs + 1);
	printf("Args: \n");

	struct ASTRoot args = *call.args;

	u64 num_args = args.number_of_nodes;

	for (u64 i = 0; i < num_args; i++)
	{
		struct ASTNode node = args.nodes[i];
		print_AST_node(node, 0, num_tabs + 2);
	}
}

void print_AST_return(struct ASTNode node, u8 num_tabs)
{
	printf("Return Statement: \n");
	print_AST_node(*node.return_expression, 1, num_tabs + 1);
}

void print_AST_if(struct ASTNode node, u8 num_tabs)
{
	printf("If statement\n");

	struct ASTIfStatement if_statement = *node.if_statement;

	print_tabs(num_tabs + 1);
	printf("Condition:\n");

	struct ASTNode cond = *if_statement.condition;
	print_AST_node(cond, 0, num_tabs + 2);

	struct ASTRoot block = *if_statement.block;
	print_tabs(num_tabs + 1);
	printf("Block:\n");

	for (u64 i = 0; i < block.number_of_nodes; i++)
	{
		struct ASTNode node = block.nodes[i];
		print_AST_node(node, 1, num_tabs + 2);
	}

	if (if_statement.else_block == NULL)
		return;

	struct ASTNode else_block = *if_statement.else_block;

    print_tabs(num_tabs + 1);
    printf("Else:\n");

	if (else_block.type == AST_BLOCK)
	{
		struct ASTRoot among = *else_block.block;

		for (u64 i = 0; i < among.number_of_nodes; i++)
		{
			struct ASTNode jorts = among.nodes[i];
			print_AST_node(jorts, 1, num_tabs + 2);
		}
	}
	else
	{
		print_AST_node(else_block, 1, num_tabs + 2);
	}
    
}

void print_AST_block(struct ASTNode node, u8 num_tabs)
{
	//print_tabs(num_tabs);
	printf("Block:\n");

	struct ASTRoot block = *node.block;

	for (u64 i = 0; i < block.number_of_nodes; i++)
	{
		struct ASTNode node = block.nodes[i];
		print_AST_node(node, 1, num_tabs + 1);
	}
}

void print_AST_loop(struct ASTNode node, u8 num_tabs)
{
	struct ASTLoop loop = *node.loop;
	printf("Loop:\n");
	
	struct ASTNode block = *loop.block;
	print_AST_node(block, 0, num_tabs + 1);
}

// TODO redo entire print system to just do the tabs here in a standard way or smthn
void print_AST_node(struct ASTNode node, u8 new_line, u8 num_tabs)
{
	print_tabs(num_tabs);
    switch (node.type)
    {
        case AST_IDENTIIFER:
            printf("Ident: %s", node.string);
			if (new_line)
				printf("\n");
            break;
        case AST_IMPORT:
            printf("import statement: (module = \"%s\")\n", node.string);
            break;
        case AST_FUNC_DEF:
            print_AST_func_def(node, num_tabs);
            break;
		case AST_FUNC_CALL:
			print_AST_func_call(node, num_tabs);
			break;
		case AST_VAR_DEF:
			print_AST_var_def(node, num_tabs);
			break;
		case AST_ARRAY_TYPE:
			print_AST_array_type(node, num_tabs);
			break;
		case AST_BINARY_OP:
			print_binary_op(node, num_tabs);
			break;
		case AST_LITERAL:
			print_AST_literal(node);
			break;	
		case AST_RETURN:
			print_AST_return(node, num_tabs);
			break;
		case AST_IF:
			print_AST_if(node, num_tabs);
			break;
		case AST_BLOCK:
			print_AST_block(node, num_tabs);
			break;
		case AST_LOOP:
			print_AST_loop(node, num_tabs);
			break;
        default:
            printf("Node type: %d\n", node.type);
            break;
    }
}

void print_AST(struct ASTRoot* root)
{
    for (u64 i = 0; i < root->number_of_nodes; i++)
    {
        struct ASTNode node = root->nodes[i];
        print_AST_node(node, 1, 0);
    }
}