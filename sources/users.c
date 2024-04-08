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

            connect_user_to_channel(users[i], channel);


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



void display_users()
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id != -1)
        {
            printf("\nUser %d is connected on channel %d\n", users[i].id, users[i].currentChannel);
        }
    }
}

void connect_user_to_channel(User user, int channel_id)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == user.id)
        {
            users[i].currentChannel = channel_id;
            printf("User %d connected to channel %d\n", user.id, channel_id);
        }
    }
}




User get_user_by_socket(socket_t socket)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].socket.fd == socket.fd)
        {
            return users[i];
        }
    }
    return users[0];
}


// get all users
User *get_users()
{
    return users;
}

bool user_exists(int id)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == id && users[i].id != -1)
        {
            return true;
        }
    }
    return false;
}



