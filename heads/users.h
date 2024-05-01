/**
 *	\file		users.h
 *	\brief	    Fichier d'en-tête du module des utilisateurs
 *	\author		Hakim FIDJEL
 *	\date		3 avril 2024
 *	\version	1.0
 */
#ifndef USERS_H
#define USERS_H
/*
*****************************************************************************************
*	\noop		I N C L U D E S   S P E C I F I Q U E S
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "./session.h"
#include "./data.h"
/*
*****************************************************************************************
*	\noop		S T R C T U R E S   DE   D O N N E E S
*/
#define MAX_USERS 10 /**< Nombre maximum d'utilisateurs	*/

/**
 * @brief Structure User
 * @struct User
 * @param currentChannel canal courant de l'utilisateur
 * @param socket socket de l'utilisateur
 * @param name nom de l'utilisateur
 * @param id id de l'utilisateur
 */
typedef struct
{
    int currentChannel; /**< canal courant de l'utilisateur	*/
    socket_t socket;    /**< socket de l'utilisateur			*/
    char name[50];      /**< nom de l'utilisateur			*/
    int id;             /**< id de l'utilisateur			*/
} User;

/*
*****************************************************************************************
*	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
*/
/**
 *	\fn			void init_users(users *u)
 *	\brief		initialisation du tableau d'utilisateurs
 *	\note	    Cette fonction initialise le tableau d'utilisateurs
 *	\result	    void
 */
void init_users();

/**
 * @brief Add a user to the users array
 * @fn User add_user(socket_t socket, int channel, char name[50])
 * @param socket The socket of the user
 * @param channel The channel the user is connected to
 * @param name The name of the user
 * @return User* 
 */
User *add_user(socket_t socket, int channel, char name[50]);

/**
 * @brief Remove a user from the users array
 * @fn void remove_user(User user)
 * @param user The user to remove
 */
void remove_user(User user);

/**
 * @brief Display all users
 * @fn void display_users()
 * @param void
 */
void display_users();

/**
 * @brief Connect a user to a channel
 * @fn void connect_user_to_channel(User *user, int channel_id)
 * @param user The user to connect
 * @param channel_id The channel to connect the user to
 */
void connect_user_to_channel(User *user, int channel_id);

/**
 * @brief Get the user by the socket
 * @fn User get_user_by_socket(socket_t socket)
 * @param socket The socket of the user
 * @return User 
 */
User get_user_by_socket(socket_t socket);

/**
 * @brief Get the users 
 * @fn User *get_users()
 * @param void
 * @return User* 
 */
User *get_users();

/**
 * @brief Test if a user exists
 * @fn bool user_exists(int id)
 * @param id The id of the user
 * @return true 
 * @return false 
 */
bool user_exists(int id);

/**
 * @brief Get the user by id
 * @fn User *get_user_by_id(int id)
 * @param id The id of the user
 * @return User* 
 */
User *get_user_by_id(int id);

#endif /* USERS_H */
