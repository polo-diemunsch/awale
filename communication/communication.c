#include <string.h>
#include <stdlib.h>

#include "communication.h"

size_t serialize_message(Message message, char *buffer)
{
    size_t n = 0;

    *buffer = message.type;
    n++;

    n += write_string_to_buffer(message.content, buffer + n);

    if (message.from != NULL)
    {
        n += write_string_to_buffer(message.from, buffer + n);
    }
    else
    {
        *(buffer + n) = '\0';
        n++;
    }

    if (message.to == NULL)
    {
        n += write_string_to_buffer(message.to, buffer + n);
    }
    else
    {
        *(buffer + n) = '\0';
        n++;
    }

    return n;
}

void unserialize_message(Message * message, char *buffer)
{
    char *pos = buffer;

    message->type = (MessageType) *(pos++);

    pos = read_string_from_buffer(&(message->content), pos);
    pos = read_string_from_buffer(&(message->from), pos);
    pos = read_string_from_buffer(&(message->to), pos);
}

size_t write_string_to_buffer(char *string, char *buffer)
{
    size_t n = strlen(string) + 1;
    strcpy(buffer, string);
    return n;
}

char * read_string_from_buffer(char **string, char *buffer)
{
    if (*string != NULL)
    {
        free(*string);
    }

    size_t length = strlen(buffer) + 1;

    if (length > 1)
    {
        *string = malloc(length * sizeof(char));
        strcpy(*string, buffer);
    }

    buffer += length;

    return buffer;
}
