#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "tokenizer.h"

int precedence(char op);

void infix_to_postfix(Token infix[EQUATION_LEN + 1], Token postfix[EQUATION_LEN + 1]);

int evaluate_expression(Token tokens[EQUATION_LEN + 1]);

int evaluate_string(const char *buf, int len, double *result);

int process_line(char *line);

#endif
