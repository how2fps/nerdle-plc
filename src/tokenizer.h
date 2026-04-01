#ifndef TOKENIZER_H
#define TOKENIZER_H
#define EQUATION_LEN 8
typedef struct
{
       char text[EQUATION_LEN];
       int is_number;
} Token;

char *tokenize_expression(const char *expression, Token tokens[EQUATION_LEN + 1]);

#endif