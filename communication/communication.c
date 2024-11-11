#include <string.h>
#include <stdlib.h>

#include "communication.h"

size_t write_string_to_buffer(const char *string, char *buffer)
{
    size_t length = strlen(string) + 1;
    strcpy(buffer, string);
    return length;
}

size_t read_string_from_buffer(char **string, const char *buffer)
{
    if (*string != NULL)
        free(*string);

    size_t length = strlen(buffer) + 1;

    *string = malloc(length * sizeof(char));
    strcpy(*string, buffer);

    return length;
}
