/**
 *	\file		users.c
 *	\brief		Spécification de la couche Users
 *	\author		Hakim FIDJEL
 *	\date		25 mars 2024
 *	\version	1.0
 */

#include "../heads/users.h"

User users[MAX_USERS]; // Tableau des utilisateurs
int USER_ID = 0;      // ID de l'utilisateur

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

/**
 * @brief Add a user to the users array
 * @fn User add_user(socket_t socket, int channel, char name[50])
 * @param socket The socket of the user
 * @param channel The channel the user is connected to
 * @param name The name of the user
 * @return User*
 */
User *add_user(socket_t socket, int channel, char name[50])
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == -1)
        {
            users[i].id = USER_ID;
            users[i].socket = socket;
            strcpy(users[i].name, name);
            printf("[add_user] %s added\n", name);

            connect_user_to_channel(&users[i], channel);

            USER_ID++;
            return &users[i]; // Retourner l'adresse de l'utilisateur ajouté
        }
    }
    return NULL; // Retourner NULL si aucun utilisateur n'a été ajouté
}

/**
 * @brief Remove a user from the users array
 * @fn void remove_user(User user)
 * @param user The user to remove
 */
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

/**
 * @brief Display all users
 * @fn void display_users()
 * @param void
 */
void display_users()
{
    printf("\n*******************************************\n");
    printf("[display_users] Name | ID | Current Channel\n");
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id != -1)
        {
            printf("%s | %d | %d\n", users[i].name, users[i].id, users[i].currentChannel);
        }
    }
    printf("\n*******************************************\n");

    return;
}

/**
 * @brief Connect a user to a channel
 * @fn void connect_user_to_channel(User *user, int channel_id)
 * @param user The user to connect
 * @param channel_id The channel to connect the user to
 */
void connect_user_to_channel(User *user, int channel_id)
{
    user->currentChannel = channel_id;
    printf("[connect_user_to_channel] %s connected to channel %d\n", user->name, channel_id);

    return;
}

/**
 * @brief Get the user by the socket
 * @fn User get_user_by_socket(socket_t socket)
 * @param socket The socket of the user
 * @return User
 */
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

/**
 * @brief Get the user by id
 * @fn User *get_user_by_id(int id)
 * @param id The id of the user
 * @return User*
 */
User *get_user_by_id(int id)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].id == id)
        {
            return &users[i];
        }
    }
    return NULL;
}

/**
 * @brief Get the users
 * @fn User *get_users()
 * @param void
 * @return User*
 */
User *get_users()
{
    return users;
}

/**
 * @brief Test if a user exists
 * @fn bool user_exists(int id)
 * @param id The id of the user
 * @return true
 * @return false
 */
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