#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Custom implementation of strndup
char *my_strndup(const char *str, size_t n) {
    char *result;
    size_t len = strlen(str);
    if (len > n) len = n;
    result = malloc(len + 1);
    if (!result) return NULL;
    result[len] = '\0';
    return memcpy(result, str, len);
}

// Define the structure for an expression node
typedef struct Expr {
    enum { NUMBER, IDENT, FUNC_CALL, OP, NEG } type;  // Type of expression
    union {
        double number;  // If the expression is a number, store it here
        char *ident;    // If the expression is an identifier, store its name here
        struct { struct Expr *left, *right; char op; } op;  // If it's an operation, store the operator and its operands
        struct { char *fname; struct Expr *arg; } func_call;  // If it's a function call, store the function name and argument
    };
} Expr;

// Define the structure for a token node (linked list of tokens)
typedef struct Token {
    enum { TOKEN_NUMBER, TOKEN_IDENT, TOKEN_OP, TOKEN_LPAR, TOKEN_RPAR, TOKEN_FUNC } type;  // Type of token
    union {
        double number;  // If the token is a number, store it here
        char *ident;    // If the token is an identifier, store its name here
        char op;        // If the token is an operator, store it here
    };
    struct Token *next;  // Pointer to the next token in the list
} Token;

// Function prototypes
Token *tokenize(const char *input);  // Tokenizes the input string
Expr *parse_expr(Token **tokens);  // Parses the token list into an expression tree
Expr *parse_term(Token **tokens);  // Parses terms (handles * and /)
Expr *parse_factor(Token **tokens);  // Parses factors (handles numbers, identifiers, parentheses)
double eval(Expr *expr);  // Evaluates the expression tree
void free_expr(Expr *expr);  // Frees the memory used by the expression tree
void free_tokens(Token *tokens);  // Frees the memory used by the token list

// Lexer Implementation: Tokenizes the input string into a linked list of tokens
Token *tokenize(const char *input) {
    Token *head = NULL, **current = &head;  // Initialize the token list
    while (*input) {  // Iterate over the input string
        while (isspace(*input)) input++;  // Skip whitespace
        Token *token = (Token *)malloc(sizeof(Token));  // Allocate memory for a new token
        token->next = NULL;
        if (isdigit(*input) || (*input == '.' && isdigit(input[1]))) {  // Handle numbers
            token->type = TOKEN_NUMBER;
            token->number = strtod(input, (char **)&input);  // Convert string to double
        } else if (isalpha(*input)) {  // Handle identifiers
            const char *start = input;
            while (isalnum(*input)) input++;  // Read the entire identifier
            size_t len = input - start;
            token->type = TOKEN_IDENT;
            token->ident = my_strndup(start, len);  // Copy the identifier into the token
        } else if (strchr("+-*/^", *input)) {  // Handle operators
            token->type = TOKEN_OP;
            token->op = *input++;
        } else if (*input == '(') {  // Handle left parentheses
            token->type = TOKEN_LPAR;
            input++;
        } else if (*input == ')') {  // Handle right parentheses
            token->type = TOKEN_RPAR;
            input++;
        } else {  // Handle invalid input
            free(token);  // Free the allocated memory for the invalid token
            while (head) {  // Free all previously allocated tokens
                Token *next = head->next;
                free(head);
                head = next;
            }
            return NULL;
        }
        *current = token;  // Add the token to the list
        current = &token->next;
    }
    return head;  // Return the head of the token list
}

// Parse a factor (numbers, identifiers, parentheses, or function calls)
Expr *parse_factor(Token **tokens) {
    Token *token = *tokens;  // Get the current token
    Expr *result;
    if (token->type == TOKEN_NUMBER) {  // Handle numbers
        result = (Expr *)malloc(sizeof(Expr));  // Allocate memory for the expression node
        result->type = NUMBER;
        result->number = token->number;  // Store the number in the expression node
        *tokens = token->next;  // Move to the next token
    } else if (token->type == TOKEN_LPAR) {  // Handle parentheses
        *tokens = token->next;  // Skip the left parenthesis
        result = parse_expr(tokens);  // Parse the expression inside the parentheses
        if ((*tokens)->type != TOKEN_RPAR) {
            fprintf(stderr, "Expected closing parenthesis\n");
            exit(1);
        }
        *tokens = (*tokens)->next;  // Skip the right parenthesis
    } else {
        fprintf(stderr, "Unexpected token\n");
        exit(1);
    }
    return result;
}

// Parse terms (factors separated by * or /)
Expr *parse_term(Token **tokens) {
    Expr *result = parse_factor(tokens);  // Parse the first factor
    while (*tokens && (*tokens)->type == TOKEN_OP && ((*tokens)->op == '*' || (*tokens)->op == '/')) {
        Token *token = *tokens;
        Expr *new_result = (Expr *)malloc(sizeof(Expr));  // Allocate memory for the new expression node
        new_result->type = OP;  // Set the node type to operation
        new_result->op.op = token->op;  // Store the operator
        new_result->op.left = result;  // The left operand is the current result
        *tokens = token->next;
        new_result->op.right = parse_factor(tokens);  // Parse the right operand (next factor)
        result = new_result;  // Update the result to the new expression
    }
    return result;
}

// Parse expressions (terms separated by + or -)
Expr *parse_expr(Token **tokens) {
    Expr *result = parse_term(tokens);  // Parse the first term
    while (*tokens && (*tokens)->type == TOKEN_OP && ((*tokens)->op == '+' || (*tokens)->op == '-')) {
        Token *token = *tokens;
        Expr *new_result = (Expr *)malloc(sizeof(Expr));  // Allocate memory for the new expression node
        new_result->type = OP;  // Set the node type to operation
        new_result->op.op = token->op;  // Store the operator
        new_result->op.left = result;  // The left operand is the current result
        *tokens = token->next;
        new_result->op.right = parse_term(tokens);  // Parse the right operand (next term)
        result = new_result;  // Update the result to the new expression
    }
    return result;
}

// Evaluate the expression tree
double eval(Expr *expr) {
    switch (expr->type) {
        case NUMBER: return expr->number;  // Return the number if it's a number node
        case OP: {
            double left = eval(expr->op.left);  // Evaluate the left operand
            double right = eval(expr->op.right);  // Evaluate the right operand
            switch (expr->op.op) {  // Perform the operation based on the operator
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': return left / right;
                case '^': return pow(left, right);  // Handle exponentiation
                default: fprintf(stderr, "Unknown operator\n"); exit(1);
            }
        }
        default: fprintf(stderr, "Unknown expression type\n"); exit(1);
    }
}

// Free the memory used by the expression tree
void free_expr(Expr *expr) {
    if (expr->type == OP) {
        free_expr(expr->op.left);  // Free the left operand
        free_expr(expr->op.right);  // Free the right operand
    }
    free(expr);  // Free the expression node itself
}

// Free the memory used by the token list
void free_tokens(Token *tokens) {
    while (tokens) {
        Token *next = tokens->next;
        if (tokens->type == TOKEN_IDENT) {
            free(tokens->ident);  // Free the identifier name
        }
        free(tokens);  // Free the token
        tokens = next;
    }
}

// Entry point of the program
int main() {
    char input[256];  // Buffer for the input string
    printf("Enter an expression: ");
    fgets(input, sizeof(input), stdin);  // Read input from the user

    Token *tokens = tokenize(input);  // Tokenize the input
    if (!tokens) {
        fprintf(stderr, "Error tokenizing input.\n");
        return 1;
    }

    Token *token_list = tokens;
    Expr *expr = parse_expr(&token_list);  // Parse the token list into an expression tree
    double result = eval(expr);  // Evaluate the expression tree

    printf("Result: %f\n", result);  // Print the result

    free_expr(expr);  // Free the memory used by the expression tree
    free_tokens(tokens);  // Free the memory used by the token list

    return 0;
}
