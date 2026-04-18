#include "file_reader.h"

/*
 * Opens the file at the given path in read mode ("r") and returns
 * the resulting FILE pointer.
 *
 * Returns NULL if the file could not be opened
 */
FILE *read_file(const char *file_name)
{
       FILE *fp = fopen(file_name, "r");

       if (fp == NULL)
       {
              return NULL;
       }

       return fp;
}
