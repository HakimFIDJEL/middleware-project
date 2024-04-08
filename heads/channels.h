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


/**
 *	\typedef	
 *	\brief		Définition du type de données user_t
 */
typedef struct {
    int id;							/**< id du channel			*/
    User users[MAX_USERS];					/**< tableau des utilisateurs du channel	*/
    User host;                        /**< hôte du channel			*/
} Channel;




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
void init_channels();

/**
 *	\fn			user_t add_user(users users, int id, int socket_id, int channel)
 *	\brief		Ajout d'un utilisateur
 *	\param		int id			id de l'utilisateur
 *	\param		int socket_id	id de la socket de l'utilisateur
 *	\param		int channel		id du canal courant de l'utilisateur
 *	\note	    Cette fonction ajoute un utilisateur au tableau
 *	\result	    user_t
 */
Channel add_channel(int id, User host);

/**
 *	\fn			void remove_user(users *u, int id)
 *	\brief		Suppression d'un utilisateur
 *	\param		user_t user		pointeur sur l'utilisateur à supprimer
 *	\note	    Cette fonction supprime un utilisateur du tableau
 *	\result	    void
 */
void remove_channel(Channel channel);


bool isUserInChannel(User user, Channel channel);



void displayChannels();



#endif /* CHANNELS_H */
