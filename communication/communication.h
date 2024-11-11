#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>

#define CRLF        "\r\n"
#define PORT        1977
#define MAX_CLIENTS 100

#define BUF_SIZE    1024

typedef enum {
    MESSAGE,
    // GAME...
} CommunicationType;

size_t write_string_to_buffer(const char *string, char *buffer);

size_t read_string_from_buffer(char **string, const char *buffer);

#endif /* guard */
