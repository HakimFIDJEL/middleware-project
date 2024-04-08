/**
 *	\file		display.h
 *	\brief	    Fichier d'en-tête du module de gestion de l'affichage
 *	\author		Younes Boughriet
 *	\date		3 avril 2024
 *	\version	1.0
 */
#ifndef DISPLAY_H
#define DISPLAY_H
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

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 100


typedef struct {
    char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
    int message_count;
    char username[100];
} Client;


void add_message(Client* client, char* message);

int print_messages(Client* client, int flag_start_client);

#endif /* DISPLAY_H */
