#include "../heads/users.h"


User users[MAX_USERS];

void init_users()
{
    for (int i = 0; i < 10; i++)
    {
        users[i].id = -1;
        users[i].socket_id = -1;
        users[i].currentChannel = -1;
    }
    printf("Users initialized\n");
}

User add_user(int id, int socket_id, int channel)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == -1)
        {
            users[i].id = id;
            users[i].socket_id = socket_id;
            users[i].currentChannel = channel;
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
            users[i].socket_id = -1;
            users[i].currentChannel = -1;
        }
    }
}