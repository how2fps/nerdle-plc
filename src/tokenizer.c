#include "tokenizer.h"
#include <ctype.h>

char *tokenize_expression(const char *expression, Token tokens[EQUATION_LEN + 1])
{
       const char *ptr = expression;
       int i = 0;
       int char_idx;
       while (*ptr != '\0' && i < EQUATION_LEN + 1)
       {
              if (isdigit(*ptr))
              {
                     char_idx = 0;
                     tokens[i].is_number = 1;
                     while (isdigit(*ptr))
                     {
                            if (char_idx < EQUATION_LEN)
                            {
                                   tokens[i].text[char_idx] = *ptr;
                                   char_idx++;
                            }
                            ptr++;
                     }
                     tokens[i].text[char_idx] = '\0';
                     i++;
              }
              else if (isspace(*ptr))
              {
                     ptr++;
              }
              else
              {
                     tokens[i].is_number = 0;
                     tokens[i].text[0] = *ptr;
                     tokens[i].text[1] = '\0';
                     i++;
                     ptr++;
              }
       }
       if (i <= EQUATION_LEN)
       {
              tokens[i].text[0] = '\0';
       }
       return (char *)expression;
}