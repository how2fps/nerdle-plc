#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "evaluator.h"
#include "game_ui.h"

/*
 * Transition function for the FSM. Given the current state and the next
 * character in the input, returns the state the machine moves to.
 *
 * States:
 *   STATE_START - before any character has been read
 *   STATE_NUM_LEFT - reading digits on the left-hand side of '='
 *   STATE_OPERATOR - just read an arithmetic operator
 *   STATE_EQUALS - just read the '=' sign
 *   STATE_NUM_RIGHT - reading digits on the right-hand side of '='
 *   STATE_INVALID - sink/error state; any further input stays here
 */
static FSMState next_state(FSMState current, char c)
{
       int is_digit = isdigit((unsigned char)c);
       int is_operator = (c == '+' || c == '-' || c == '*' || c == '/');
       int is_equal = (c == '=');

       switch (current)
       {
       case STATE_START:
              /* equations must begin with a digit */
              if (is_digit)
                     return STATE_NUM_LEFT;
              return STATE_INVALID;

       case STATE_NUM_LEFT:
              /* on the LHS: keep reading digits, accept an operator, or accept '=' */
              if (is_digit)
                     return STATE_NUM_LEFT;
              if (is_operator)
                     return STATE_OPERATOR;
              if (is_equal)
                     return STATE_EQUALS;
              return STATE_INVALID;

       case STATE_OPERATOR:
              /* after an operator, the next character must be a digit */
              if (is_digit)
                     return STATE_NUM_LEFT;
              return STATE_INVALID;

       case STATE_EQUALS:
              /* after '=', the next character must be a digit */
              if (is_digit)
                     return STATE_NUM_RIGHT;
              return STATE_INVALID;

       case STATE_NUM_RIGHT:
              /* on the RHS: only more digits are allowed */
              if (is_digit)
                     return STATE_NUM_RIGHT;
              return STATE_INVALID;

       case STATE_INVALID:
       default:
              return STATE_INVALID;
       }
}

/*
 * Runs the FSM over the full equation string and applies additional
 * structural checks:
 *   1. No character may drive the FSM to STATE_INVALID.
 *   2. The string length must be exactly EQUATION_LEN.
 *   3. The FSM must end in STATE_NUM_RIGHT (accepting state).
 *   4. The '=' sign must not appear at the very start or very end.
 *
 * Prints a descriptive rejection message for each failure case.
 *
 * Returns:
 *   1 - string is a syntactically valid equation
 *   0 - string fails one or more validation rules
 */
int validate_equation(const char *expr)
{
       FSMState state = STATE_START;
       int count = 0; /* total characters processed */
       int equal_position = -1; /* index of '=' in the string, -1 if not found */
       int i;

       if (expr == NULL)
              return 0;

       for (i = 0; expr[i] != '\0'; i++)
       {
              state = next_state(state, expr[i]);
              count++;

              /* record the position of the first '=' encountered */
              if (expr[i] == '=' && equal_position == -1)
              {
                     equal_position = i;
              }

              /* any invalid transition is an immediate rejection */
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
