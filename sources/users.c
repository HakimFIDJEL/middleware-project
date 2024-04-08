#include "../heads/users.h"


User users[MAX_USERS];
int USER_ID = 0;

void init_users()
{
    for (int i = 0; i < 10; i++)
    {
        users[i].id = -1;
        users[i].currentChannel = -1;
    }
    printf("Users initialized\n");
}

User add_user(socket_t socket, int channel)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == -1)
        {
            users[i].id = USER_ID;
            users[i].socket = socket;
            users[i].currentChannel = channel;
            USER_ID ++;
            return users[i];
        }
    }
    return users[0];
}

void remove_user(User user)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == user.id)
        {
            users[i].id = -1;
            users[i].currentChannel = -1;
            printf("User %d removed\n", user.id);
        }
    }
}



void displayUsers()
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id != -1)
        {
            printf("\nUser %d is connected on channel %d\n", users[i].id, users[i].currentChannel);
        }
    }
}