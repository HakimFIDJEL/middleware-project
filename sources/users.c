#include "../heads/users.h"


User users[MAX_USERS];
Message messages[MAX_MESSAGES];
int USER_ID = 0;
int MESSAGE_ID = 0;

void init_users()
{
    for (int i = 0; i < 10; i++)
    {
        users[i].id = -1;
        users[i].socket.fd = -1;
        users[i].name[0] = '\0';
        users[i].currentChannel = -1;
    }
    printf("[init_users] Users initialized\n");
}


User add_user(socket_t socket, int channel, char name[50])
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == -1)
        {
            users[i].id = USER_ID;
            users[i].socket = socket;
            strcpy(users[i].name, name);
            printf("[add_user] %s added\n", name);
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
            printf("[remove_user] %s removed\n", user.name);
        }
    }
}



void display_users()
{
    printf("*******************************************\n");
    printf("[display_users] Name | ID | Current Channel\n");
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id != -1)
        {
            printf("%s | %d | %d\n", users[i].name, users[i].id , users[i].currentChannel );
        }
    }
    printf("*******************************************\n"); 

    return;
}

void connect_user_to_channel(User user, int channel_id)
{
    user.currentChannel = channel_id;
    printf("[connect_user_to_channel] %s connected to channel %d\n", user.name, channel_id);
    
    return;
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



