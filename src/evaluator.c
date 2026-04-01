#include "evaluator.h"
#include <stdlib.h>
#include <stdio.h>

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
                            incomingPrecedence = precedence(infix[i].text);
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

void process_line(char *line)
{
       char *equals_ptr;
       char *expression;
       char *expected_str;
       int calculated_result;
       int expected_result;
       Token tokens[EQUATION_LEN + 1];
       Token postfix[EQUATION_LEN + 1];

       equals_ptr = strchr(line, '=');

       if (equals_ptr != NULL)
       {
              *equals_ptr = '\0';

              expression = line;
              expected_str = equals_ptr + 1;

              tokenize_expression(expression, tokens);
              infix_to_postfix(tokens, postfix);
              calculated_result = evaluate_expression(postfix);

              expected_result = atoi(expected_str);

              if (calculated_result == expected_result)
              {
                     printf("Correct! %d == %d\n", calculated_result, expected_result);
              }
              else
              {
                     printf("Wrong! Calculated %d, but expected %d\n", calculated_result, expected_result);
              }
       }
}