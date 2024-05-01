/**
 *	\file		ncurses.h
 *	\brief		Fichier d'en-tête du module de gestion de l'affichage
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
#define MAX_MSG 100 // Taille maximale d'un message

// Déclaration des fenêtres

/*
*****************************************************************************************
*	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
*/

/**
 * @brief Initialize ncurses
 * @fn void init_ncurses()
 * @param void
 * @return void
 */
void init_ncurses();

/**
 * @brief Create windows
 * @fn void create_windows()
 * @param void
 * @return void
 */
void create_windows();

/**
 * @brief Display the main window
 * @fn void config_colors()
 * @param void
 * @return void
 */
void config_colors();

/**
 * @brief Display message in the window specified
 * @fn void display_message(WINDOW *win, char *msg, int root)
 * @param win the window to display the message
 * @param msg the message to display
 * @param root 1 if the message is from the server, 0 otherwise
 */
void display_message(WINDOW *win, char *msg, int root);

/**
 * @brief The main loop of the ncurses interface
 * @fn void main_loop_ncurses()
 * @param void
 */
void main_loop_ncurses();

/**
 * @brief Cleanup ncurses
 * @fn void cleanup()
 * @param void
 */
void cleanup();

#endif /* NCURSES_H */