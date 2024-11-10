#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>

#define CRLF        "\r\n"
#define PORT        1977
#define MAX_CLIENTS 100

#define BUF_SIZE    1024

typedef enum {
    INFORMATION,
    ERROR,
    MESSAGE_SENT,
} MessageType;

typedef struct {
    MessageType type;
    char *content;
    char *from;
    char *to;
} Message;

size_t serialize_message(Message message, char *buffer);

void unserialize_message(Message * message, char *buffer);

size_t write_string_to_buffer(char *string, char *buffer);

char *read_string_from_buffer(char **string, char *buffer);

#endif /* guard */
