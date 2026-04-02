#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "tokenizer.h"

void process_line(char *line);

void infix_to_postfix(Token infix[EQUATION_LEN + 1], Token postfix[EQUATION_LEN + 1]);

int evaluate_expression(Token tokens[EQUATION_LEN + 1]);

int precedence(char op);

int evaluate_lhs(const char *buf, int len, double *result);

#endif