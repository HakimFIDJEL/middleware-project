/**
 *	\file		channels.h
 *	\brief	    Fichier d'en-tête pour la gestion des channels
 *	\author		Hakim FIDJEL
 *	\date		3 avril 2024
 *	\version	1.0
 */
#ifndef CHANNELS_H
#define CHANNELS_H
/*
*****************************************************************************************
*	\noop		I N C L U D E S   S P E C I F I Q U E S
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "./users.h"
/*
*****************************************************************************************
*	\noop		S T R C T U R E S   DE   D O N N E E S
*/
#define MAX_CHANNELS 10

/**
 * @brief Définition de la structure Channel
 * @struct Channel
 * @param id id du channel
 * @param users[MAX_USERS] utilisateurs du channel
 * @param host hôte du channel
 * @param name nom du channel
 */
typedef struct
{
    int id;               /**< id du channel			*/
    int users[MAX_USERS]; /**< utilisateurs du channel	*/
    User host;            /**< hôte du channel			*/
    char name[50];        /**< nom du channel			*/
} Channel;

/*
*****************************************************************************************
*	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
*/

/**
 *	\fn			void init_channels()
 *	\brief		initialisation des channels
 *	\note	    Cette fonction initialise le tableau de channels
 *	\result	    void
 */
void init_channels();

/**
 *	\fn			add_channel(int id, User host, char name[50])
 *	\brief		Ajout d'un channel
 *	\param		User host		hôte du channel
 *	\param		char name[50]	nom du channel
 *	\note	    Cette fonction ajoute un channel au tableau
 *	\result	    Channel
 */
Channel add_channel(User host, char name[50]);

/**
 * \fn		    remove_channel(Channel channel)
 * \brief		Suppression d'un channel
 * \param		channel		pointeur sur le channel à supprimer
 * \note	    Cette fonction supprime un channel du tableau
 * \result	    void
 */
void remove_channel(Channel *channel);

/**
 * \fn         is_user_allowed_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      user      utilisateur à vérifier
 * \param      channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
 */
bool is_user_allowed_in_channel(User user, Channel channel);

/**
 * \fn         is_user_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      user      utilisateur à vérifier
 * \param      channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
 */
bool is_user_in_channel(User user, Channel channel);

/**
 * @brief Get the channel by id
 * @fn Channel *get_channel_by_id(int id)
 * @param id The id of the channel
 * @return Channel* 
 */
Channel *get_channel_by_id(int id);

/**
 * @brief add user to channel
 * @fn void add_user_to_channel(User user, Channel *channel)
 * @param user The user to add
 * @param channel The channel to add the user to
 * @return void
 */
void add_user_to_channel(User user, Channel *channel);

/**
 * @brief display channels
 * @fn void display_channels(User user, buffer_t buff)
 * @param user The user to display the channels to
 * @param buff The buffer to display the channels to
 */
void display_channels(User user, buffer_t buff);

/**
 * @brief display users in channel
 * @fn void display_users_in_channel(Channel channel, buffer_t buff)
 * @param channel The channel to display the users from
 * @param buff The buffer to display the users to
 * @return void
 */
void display_users_in_channel(Channel channel, buffer_t buff);

/**
 * @brief remove user from channel
 * @fn void remove_user_from_channel(User user, Channel *channel)
 * @param user The user to remove
 * @param channel The channel to remove the user from
 * @return void
 */
void remove_user_from_channel(User user, Channel *channel);

/**
 * @brief get channels
 * @fn Channel *get_channels()
 * @return Channel* 
 */
Channel *get_channels();

#endif /* CHANNELS_H */
