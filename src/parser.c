#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "evaluator.h"

static FSMState next_state(FSMState current, char c)
{
       int is_digit = isdigit((unsigned char)c);
       int is_operator = (c == '+' || c == '-' || c == '*' || c == '/');
       int is_equal = (c == '=');

       switch (current)
       {
       case STATE_START:
              if (is_digit)
                     return STATE_NUM_LEFT;
              return STATE_INVALID;

       case STATE_NUM_LEFT:
              if (is_digit)
                     return STATE_NUM_LEFT;
              if (is_operator)
                     return STATE_OPERATOR;
              if (is_equal)
                     return STATE_EQUALS;
              return STATE_INVALID;

       case STATE_OPERATOR:
              if (is_digit)
                     return STATE_NUM_LEFT;
              return STATE_INVALID;

       case STATE_EQUALS:
              if (is_digit)
                     return STATE_NUM_RIGHT;
              return STATE_INVALID;

       case STATE_NUM_RIGHT:
              if (is_digit)
                     return STATE_NUM_RIGHT;
              return STATE_INVALID;

       case STATE_INVALID:
       default:
              return STATE_INVALID;
       }
}

int validate_equation(const char *expr)
{
       FSMState state = STATE_START;
       int count = 0;
       int equal_position = -1;
       int i;
       double lhs_val = 0.0;
       double rhs_val = 0.0;
       char lhs_buf[EQUATION_LEN + 1];
       char rhs_buf[EQUATION_LEN + 1];
       int lhs_len;
       int rhs_len;

       if (expr == NULL)
              return 0;

       for (i = 0; expr[i] != '\0'; i++)
       {
              state = next_state(state, expr[i]);
              count++;

              if (expr[i] == '=' && equal_position == -1)
              {
                     equal_position = i;
              }

              if (state == STATE_INVALID)
              {
                     printf("Rejected at char %d ('%c') -> SINK\n", i, expr[i]);
                     return 0;
              }
       }

       if (count != EQUATION_LEN)
       {
              printf("Rejected: length %d != %d\n", count, EQUATION_LEN);
              return 0;
       }

       if (state != STATE_NUM_RIGHT)
       {
              printf("Rejected: ended in non-accepting state\n");
              return 0;
       }

       if (equal_position < 1 || equal_position >= count - 1)
       {
              printf("Rejected: '=' in invalid position\n");
              return 0;
       }

       lhs_len = equal_position;
       rhs_len = count - equal_position - 1;

       strncpy(lhs_buf, expr, lhs_len);
       lhs_buf[lhs_len] = '\0';
       strncpy(rhs_buf, expr + equal_position + 1, rhs_len);
       rhs_buf[rhs_len] = '\0';

       if (evaluate_lhs(lhs_buf, lhs_len, &lhs_val) != 0)
       {
              printf("Rejected: LHS evaluation failed\n");
              return 0;
       }

       if (evaluate_lhs(rhs_buf, rhs_len, &rhs_val) != 0)
       {
              printf("Rejected: RHS evaluation failed\n");
              return 0;
       }

       if (lhs_val != rhs_val)
       {
              printf("Rejected: %s=%.0f but RHS=%.0f\n",
                     lhs_buf, lhs_val, rhs_val);
              return 0;
       }

       return 1;
}
