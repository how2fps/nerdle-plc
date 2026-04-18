#include "evaluator.h"
#include "game_ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Returns the precedence level of a given arithmetic operator.
 * '*' and '/' have higher precedence (2) than '+' and '-' (1).
 * Any unrecognised character returns 0.
 * 
 * Used by infix_to_postfix() to decide when to pop the operator stack.
 */

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

/*
 * Converts an array of tokens in infix order (e.g. 3 + 4 * 2) into
 * postfix / Reverse Polish Notation (e.g. 3 4 2 * +) using the
 * shunting-yard algorithm.
 *
 * Numbers are passed straight through to the output array.
 * Operators are pushed onto a temporary stack and popped to output
 * whenever a lower- or equal-precedence operator arrives, ensuring
 * that higher-precedence operations are evaluated first.
 */
void infix_to_postfix(Token infix[EQUATION_LEN + 1], Token postfix[EQUATION_LEN + 1])
{
       int i = 0;
       int j = 0;    /* index into output (postfix) array */
       int top = -1; /* top of the operator stack (-1 = empty) */
       char stack[EQUATION_LEN + 1];
       int topPrecedence;
       int incomingPrecedence;
       for (i = 0; i < EQUATION_LEN; i++)
       {
              /* stop at the sentinel empty token */
              if (infix[i].text[0] == '\0')
              {
                     break;
              }

              if (infix[i].is_number)
              {
                     /* numbers go directly to the output */
                     postfix[j] = infix[i];
                     j++;
              }
              else
              {
                     /* operator: pop any higher/equal-precedence operators first */
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
                     /* push the incoming operator onto the stack */
                     top++;
                     stack[top] = infix[i].text[0];
              }
       }
       /* flush any remaining operators from the stack to the output */
       while (top >= 0)
       {
              postfix[j].is_number = 0;
              postfix[j].text[0] = stack[top];
              postfix[j].text[1] = '\0';
              j++;
              top--;
       }
       /* null-terminate the postfix array */
       postfix[j].text[0] = '\0';
}

/*
 * Evaluates a postfix token array using a stack-based approach.
 *
 * Numbers are pushed onto the stack.
 * When an operator is encountered, the top two values are popped,
 * the operation is applied, and the result is pushed back.
 *
 * Returns the final integer result, or -1 on division by zero.
 */
int evaluate_expression(Token tokens[EQUATION_LEN + 1])
{
       int stack[EQUATION_LEN + 1];
       int i = 0;
       int top = -1;
       int left, right, result;

       for (i = 0; i < EQUATION_LEN + 1; i++)
       {
              /* stop at the sentinel empty token */
              if (tokens[i].text[0] == '\0')
              {
                     break;
              }

              if (tokens[i].is_number)
              {
                     /* push numeric value onto the stack */
                     top++;
                     stack[top] = atoi(tokens[i].text);
              }
              else
              {
                     /* pop the two operands (right before left due to stack order) */
                     right = stack[top];
                     top--;
                     left = stack[top];
                     top--;

                     if (tokens[i].text[0] == '+')
                     {
                            result = left + right;
                     }
                     else if (tokens[i].text[0] == '-')
                     {
                            result = left - right;
                     }
                     else if (tokens[i].text[0] == '*')
                     {

                            result = left * right;
                     }
                     else if (tokens[i].text[0] == '/')
                     {
                            /* guard against division by zero */
                            if (right == 0)
                            {
                                   return -1;
                            }
                            result = left / right;
                     }
                     /* push the result back for the next operation */
                     top++;
                     stack[top] = result;
              }
       }
       return stack[top];
}

/*
 * Convenience wrapper that runs the full tokenize → postfix → evaluate
 * pipeline on a raw character buffer.
 *
 * On success, stores the computed value in *result and returns it.
 * Returns -1 (and leaves *result unchanged) on division by zero.
 */
int evaluate_string(const char *buf, int len, int *result)
{
       int answer;
       Token infix[EQUATION_LEN + 1];
       Token postfix[EQUATION_LEN + 1];

       tokenize_expression(buf, infix);
       infix_to_postfix(infix, postfix);
       answer = evaluate_expression(postfix);
       if (answer != -1)
       {
              *result = answer;
       }
       return answer;
}

/*
 * Validates whether a full equation string (e.g. "3+5=8") is mathematically
 * correct by splitting on '=', evaluating both sides, and comparing them.
 *
 * Returns:
 *   1  - equation is balanced (LHS == RHS)
 *   0  - equation is unbalanced
 *  -1  - evaluation error (e.g. division by zero)
 */
int process_line(char *line, int print_message)
{
       char *line_copy;
       char *equals_ptr;
       char *lhs_str, *rhs_str;
       int left_val, right_val;
       int result = 0;

       line_copy = (char *)malloc(strlen(line) + 1);
       strncpy(line_copy, line, strlen(line) + 1);
       equals_ptr = strchr(line_copy, '=');
       if (equals_ptr != NULL)
       {
              /* split the string into left-hand and right-hand sides */
              *equals_ptr = '\0';
              lhs_str = line_copy;
              rhs_str = equals_ptr + 1;

              /* evaluate both sides */
              if ((evaluate_string(lhs_str, strlen(lhs_str), &left_val)) == -1 || (evaluate_string(rhs_str, strlen(rhs_str), &right_val)) == -1)
              {
                     printf(COLOR_RED "Error: Division by 0!\n" COLOR_RESET);
                     free(line_copy);
                     return -1;
              }

              if (left_val == right_val)
              {
                     result = 1;
              }
              else
              {
                     if (print_message)
                     {
                            printf(COLOR_RED "Rejected: LHS: %s=%d but RHS: %d\n" COLOR_RESET, lhs_str, left_val, right_val);
                     }
                     result = 0;
              }
       }
       free(line_copy);
       return result;
}
