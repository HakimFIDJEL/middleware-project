/**
 *	\file		users.h
 *	\brief	    Fichier d'en-tête du module des utilisateurs
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

typedef struct UserNode {
    User* user;
    struct UserNode* next;
} UserNode;

/**
 *	\typedef	
 *	\brief		Définition du type de données user_t
 */
typedef struct {
    int id;							/**< id du channel			*/
    UserNode* users;					/**< Liste chaînée des utilisateurs d'un channel	*/
    User host;                        /**< hôte du channel			*/
    char name[50];                  /**< nom du channel			*/
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
*	\fn			 add_channel(int id, User host, char name[50])
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
 * \param		Channel channel		pointeur sur le channel à supprimer
 * \note	    Cette fonction supprime un channel du tableau
 * \result	    void
 */
void remove_channel(Channel channel);

/**
 * \fn         is_user_allowed_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      User user      utilisateur à vérifier
 * \param      Channel channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
*/
bool is_user_allowed_in_channel(User user, Channel channel);

/**
 * \fn         is_user_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      User user      utilisateur à vérifier
 * \param      Channel channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
*/
bool is_user_in_channel(User user, Channel channel);

/**
 * \fn         displayChannels()
 * \brief      Affiche les channels
 * \note       Cette fonction affiche les channels
 * \result     void
*/
void display_channels();


Channel get_channel_by_id(int id);

void add_user_to_channel(User user, Channel channel);





#endif /* CHANNELS_H */
