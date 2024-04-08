#include "../heads/display.h"

void add_message(Client* client, char* message) {
    if (client->message_count < MAX_MESSAGES) {
        strncpy(client->messages[client->message_count], message, MAX_MESSAGE_LENGTH);
        client->message_count++;
    }
}

int print_messages(Client* client, int flag_start_client) {
    if(flag_start_client)
        flag_start_client = 0;
    system("clear");
    for (int i = 0; i < client->message_count; i++) {
        printf("%s\n", client->messages[i]);
    }
    printf("Enter a new message: ");
    return flag_start_client;
}