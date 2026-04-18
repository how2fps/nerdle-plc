#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "evaluator.h"
#include "game_ui.h"

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
                     printf(COLOR_RED "Rejected at char %d ('%c') -> SINK\n" COLOR_RESET, i, expr[i]);
                     return 0;
              }
       }

       if (count != EQUATION_LEN)
       {
              printf(COLOR_RED "Rejected: length %d != %d\n" COLOR_RESET, count, EQUATION_LEN);
              return 0;
       }

       if (state != STATE_NUM_RIGHT)
       {
              printf(COLOR_RED "Rejected: ended in non-accepting state\n" COLOR_RESET);
              return 0;
       }

       if (equal_position < 1 || equal_position >= count - 1)
       {
              printf(COLOR_RED "Rejected: '=' in invalid position\n" COLOR_RESET);
              return 0;
       }

       return 1;
}
