/**
 *	\file		messages.h
 *	\brief	    Fichier d'en-tête du module de gestion de l'affichage
 *	\author		Younes Boughriet
 *	\date		3 avril 2024
 *	\version	1.0
 */
#ifndef MESSAGES_H
#define MESSAGES_H
/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "./channels.h"

#define tmp_path "./tmp/"
#define MAX_MESSAGES 1000


/**
 *	\typedef	
 *	\brief		Définition du type de données message
 */
typedef struct {
    int id;							/**< id du message			*/
    int channel;					/**< canal du message		*/
    int user;						/**< utilisateur du message	*/
    buffer_t buff;				/**< contenu du message	*/
    time_t timestamp;				/**< timestamp du message	*/
} Message;


// Create if not exists one file for each channel in the ./tmp directory to store messages
void init_messages(Channel *channels);

// Store a message in the correct file
void store_message(int channel, int user, buffer_t buff);

// Retrieve all messages from the file of the given channel
Message* get_messages(int channel);

// Display all messages from the file
void display_messages(Message *messages);

// Delete all messages from the file of the given channel and the file
void delete_messages(int channel);





#endif /* MESSAGES_H */
