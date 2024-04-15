/**
 *	\file		ncurses.h
 *	\brief		UI du chat
 *	\author		Younes Boughriet
 *	\date		15 avril 2024
 *	\version	1.0
 */
#ifndef NCURSES_H
#define NCURSES_H
/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <string.h>
#include "./session.h"
/*
*****************************************************************************************
 *	\noop		D E F I N I T I O N   DES   C O N S T A N T E S
 */
/**
 *	\def		MAX_BUFFER
 *	\brief		taille d'un buffer_t d'émission/réception
 */
#define MAX_MSG 100
// Déclaration des fenêtres


/*
*****************************************************************************************
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */

int main_ncurses();
/**
 *	\fn			void init_ncurses()
 *	\brief		Initilisation de ncurses
 */
void init_ncurses();
/**
 *	\fn			void create_windows()
 *	\brief		 Création des fenêtres pour le client, messages, input et logs
 */
void create_windows();
/**
 *	\fn			void config_colors()
 *	\brief		Configuration des couleurs pour les fenêtres
 */
void config_colors();
/**
 *	\fn			void display_message(WINDOW *win, char *msg)
 *	\brief		Affichage d'un message dans une fenêtre
 *	\param		win : fenêtre où afficher le message
 *	\param		msg : message à afficher
 */
void display_message(WINDOW *win, char *msg);
/**
 *	\fn			void main_loop_ncurses()
 *	\brief		Boucle principale de ncurses pour le chat (s'inspirer pour le chat principal)
 */
void main_loop_ncurses();
/**
 *	\fn			void cleanup()
 *	\brief		Nettoyage des fenêtres et fin de ncurses
 */
void cleanup();





#endif /* NCURSES_H */