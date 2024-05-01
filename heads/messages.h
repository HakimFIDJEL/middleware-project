/**
 *	\file		messages.h
 *	\brief	    Fichier d'en-tête du module de gestion de l'affichage
 *	\author		Hakim FIDJEL
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
 * @brief Définition de la structure Message
 * @struct Message
 * @param id id du message
 * @param channel canal du message
 * @param user utilisateur du message
 * @param buff contenu du message
 * @param timestamp timestamp du message
 */
typedef struct
{
    int id;           /**< id du message			*/
    int channel;      /**< canal du message		*/
    int user;         /**< utilisateur du message	*/
    buffer_t buff;    /**< contenu du message	*/
    time_t timestamp; /**< timestamp du message	*/
} Message;

/**
 * @brief Create if not exists one file for each channel in the ./tmp directory to store messages
 * @fn void init_messages(Channel *channels)
 * @param channels tableau de channels
 */
void init_messages(Channel *channels);

/**
 * @brief Store a message in the correct file
 * @fn void store_message(int channel, int user, buffer_t buff)
 * @param channel The channel where the message is sent
 * @param user The user who sent the message
 * @param buff The message to store
 */
void store_message(int channel, int user, buffer_t buff);

/**
 * @brief Get the messages object
 * @fn Message *get_messages(int channel)
 * @param channel The channel to get the messages from
 * @return Message* 
 */
Message *get_messages(int channel);

/**
 * @brief Display all messages from the file
 * @fn void display_messages(Message *messages)
 * @param messages The messages to display
 */
void display_messages(Message *messages);

/**
 * @brief Delete all messages from the file of the given channel and the file
 * @param channel The channel to delete the messages from
 */
void delete_messages(int channel);

#endif /* MESSAGES_H */
