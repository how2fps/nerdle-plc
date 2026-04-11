#include "evaluator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int precedence(char op)
{
       if (op == '*' || op == '/')
       {
              return 2;
       }
       if (op == '+' || op == '-')
       {
              return 1;
       }
       return 0;
}

void infix_to_postfix(Token infix[EQUATION_LEN + 1], Token postfix[EQUATION_LEN + 1])
{
       int i = 0;
       int j = 0;
       int top = -1;
       char stack[EQUATION_LEN + 1];
       int topPrecedence;
       int incomingPrecedence;
       for (i = 0; i < EQUATION_LEN; i++)
       {
              if (infix[i].text[0] == '\0')
              {
                     break;
              }

              if (infix[i].is_number)
              {
                     postfix[j] = infix[i];
                     j++;
              }
              else
              {
                     if (top >= 0)
                     {
                            topPrecedence = precedence(stack[top]);
                            incomingPrecedence = precedence(infix[i].text[0]);
                            if (topPrecedence >= incomingPrecedence)
                            {
                                   while (top >= 0 && precedence(stack[top]) >= incomingPrecedence)
                                   {
                                          postfix[j].is_number = 0;
                                          postfix[j].text[0] = stack[top];
                                          postfix[j].text[1] = '\0';
                                          j++;
                                          top--;
                                   }
                            }
                     }
                     top++;
                     stack[top] = infix[i].text[0];
              }
       }
       while (top >= 0)
       {
              postfix[j].is_number = 0;
              postfix[j].text[0] = stack[top];
              postfix[j].text[1] = '\0';
              j++;
              top--;
       }
       postfix[j].text[0] = '\0';
}

int evaluate_expression(Token tokens[EQUATION_LEN + 1])
{
       int stack[EQUATION_LEN + 1];
       int i = 0;
       int top = -1;
       int left, right, result;

       for (i = 0; i < EQUATION_LEN + 1; i++)
       {
              if (tokens[i].text[0] == '\0')
                     break;

              if (tokens[i].is_number)
              {
                     top++;
                     stack[top] = atoi(tokens[i].text);
              }
              else
              {
                     right = stack[top];
                     top--;
                     left = stack[top];
                     top--;

                     if (tokens[i].text[0] == '+')
                            result = left + right;
                     if (tokens[i].text[0] == '-')
                            result = left - right;
                     if (tokens[i].text[0] == '*')
                            result = left * right;
                     if (tokens[i].text[0] == '/')
                            result = left / right;

                     top++;
                     stack[top] = result;
              }
       }
       return stack[top];
}

int evaluate_string(const char *buf, int len, double *result)
{
       Token infix[EQUATION_LEN + 1];
       Token postfix[EQUATION_LEN + 1];

       tokenize_expression(buf, infix);
       infix_to_postfix(infix, postfix);
       *result = (double)evaluate_expression(postfix);
       return 0;
}

int process_line(char *line)
{
       char *line_copy = strdup(line);
       char *equals_ptr = strchr(line_copy, '=');
       char *lhs_str, *rhs_str;
       double left_val, right_val;
       int result = 0;

       if (equals_ptr != NULL)
       {
              *equals_ptr = '\0';
              lhs_str = line_copy;
              rhs_str = equals_ptr + 1;

              evaluate_string(lhs_str, strlen(lhs_str), &left_val);
              evaluate_string(rhs_str, strlen(rhs_str), &right_val);

              if (left_val == right_val)
              {
                     printf("Correct! LHS: %s=%.0f == RHS: %.0f\n", lhs_str, left_val, right_val);
                     result = 1;
              }
              else
              {
                     printf("Rejected: LHS: %s=%.0f but RHS: %.0f\n", lhs_str, left_val, right_val);
                     result = 0;
              }
       }
       free(line_copy);
       return result;
}
