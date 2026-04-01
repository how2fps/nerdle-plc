#ifndef PARSER_H
#define PARSER_H

#define EQUATION_LEN 8

typedef enum
{
    STATE_START,
    STATE_NUM_LEFT,
    STATE_OPERATOR,
    STATE_EQUALS,
    STATE_NUM_RIGHT,
    STATE_INVALID
} FSMState;

int validate_equation(const char *expr);

#endif