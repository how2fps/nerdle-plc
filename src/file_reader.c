#include "file_reader.h"

FILE *read_file(const char *file_name)
{
       FILE *fp = fopen(file_name, "r");

       if (fp == NULL)
       {
              return NULL;
       }

       return fp;
}
