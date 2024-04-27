/**
 *	\file		ncurses.c
 *	\brief		Fonctions de l'interface utilisateur du chat
 *	\author		Younes Boughriet
 *	\date		15 avril 2024
 *	\version	1.0
 */

#include <ncursesw/ncurses.h>
#include <locale.h>
#include <string.h>
#include "../heads/ncurses.h"

int height, width, main_width, side_width;
WINDOW *top_win, *bottom_win, *logs_win;

/*TEST*/

// int main() {

//     init_ncurses();
//     create_windows();
//     config_colors();
//     main_loop_ncurses();
//     cleanup();

//     return 0;
// }


/**
 *	\fn			void init_ncurses()
 *	\brief		Initilisation de ncurses
 */
void init_ncurses() {
    setlocale(LC_ALL, "");  // Pour supporter les caractères UTF-8
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, height, width);
    main_width = width * 3 / 4;
    side_width = width - main_width;
}

/**
 *	\fn			void create_windows()
 *	\brief		 Création des fenêtres pour le client, messages, input et logs
 */
void create_windows() {
    top_win = newwin(height - 3, main_width, 0, 0);
    bottom_win = newwin(3, main_width, height - 3, 0);
    logs_win = newwin(height, side_width, 0, main_width);
}

/**
 *	\fn			void config_colors()
 *	\brief		Configuration des couleurs pour les fenêtres
 */
void config_colors() {
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_RED, COLOR_MAGENTA);

    wbkgd(top_win, COLOR_PAIR(2));
    wbkgd(bottom_win, COLOR_PAIR(3));
    wbkgd(logs_win, COLOR_PAIR(2));
    scrollok(top_win, TRUE);
}

/**
 *	\fn			void display_message(WINDOW *win, char *msg)
 *	\brief		Affichage d'un message dans une fenêtre
 *	\param		win : fenêtre où afficher le message
 *	\param		msg : message à afficher
 */
void display_message(WINDOW *win, char *msg, int root) {
    //Si root est a 1, alors c'est un message du serveur donc on change la couleur du texte en rouge
    if(root == 1){
        wattron(win, COLOR_PAIR(4));
    } else {
        wattron(win, COLOR_PAIR(2));
    }
    int y, x;
    getyx(win, y, x);
    if (y == height - 4) {
        scroll(win);
    } else {
        wmove(win, y + 1, 1);
    }
    wprintw(win, "%s\n", msg);
    box(win, 0, 0); // Redraw box to ensure borders are intact
    wrefresh(win);
    wattroff(win, COLOR_PAIR(2)); // On enlève la couleur pour éviter de colorer les logs
    wattroff(win, COLOR_PAIR(1)); // On enlève la couleur pour éviter de colorer les logs
}

/**
 *	\fn			void main_loop_ncurses()
 *	\brief		Boucle principale de ncurses pour le chat (s'inspirer pour le chat principal)
 */
void main_loop_ncurses() {
    char msg[MAX_MSG];
    int ch, i = 0;

    box(top_win, 0, 0);
    box(bottom_win, 0, 0);
    box(logs_win, 0, 0);
    wrefresh(top_win);
    wrefresh(bottom_win);
    wrefresh(logs_win);

    wmove(logs_win, 1, 1);
    wprintw(logs_win, "Logs : ");
    wrefresh(logs_win);

    wmove(top_win, 1, 1);
    wprintw(top_win, "Discord Style Chat (type '/disconnect' to quit)");
    wrefresh(top_win);

    while (1) {
    wmove(bottom_win, 1, 1); // Move cursor to start of input line
    wclrtoeol(bottom_win); // Clear the input line
    box(bottom_win, 0, 0); // Redraw box to ensure borders are intact
    wrefresh(bottom_win); // Refresh the window to reflect changes

    i = 0;
    while (1) {
        ch = wgetch(bottom_win);
        if (ch == '\n') break;
        if (ch == KEY_RESIZE) {
            endwin(); // Temporarily end ncurses mode to clear buffers
            refresh(); // Refresh the standard screen to clear the ncurses formatting
            clear(); // Clear the screen from any leftovers

            getmaxyx(stdscr, height, width); // Get new dimensions
            main_width = width * 3 / 4;
            side_width = width - main_width;

            // Delete old windows
            delwin(top_win);
            delwin(bottom_win);
            delwin(logs_win);

            // Create new windows with new dimensions
            create_windows();
            config_colors();

            // Redraw borders and refresh each window
            box(top_win, 0, 0);
            box(bottom_win, 0, 0);
            box(logs_win, 0, 0);
            wrefresh(top_win);
            wrefresh(bottom_win);
            wrefresh(logs_win);
            continue; // Skip the rest of the loop to handle resizing properly
        }
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwdelch(bottom_win, 1, i + 1);
            }
        } else if (i < MAX_MSG - 1) {
            msg[i++] = ch;
            waddch(bottom_win, ch);
        }
    }
    msg[i] = '\0';

    if (strcmp(msg, "/disconnect") == 0) break;

    // Display message in top window
    display_message(top_win, msg, 0);
}

}

/**
 *	\fn			void cleanup()
 *	\brief		Nettoyage des fenêtres et fin de ncurses
 */
void cleanup() {
    delwin(top_win);
    delwin(bottom_win);
    delwin(logs_win);
    endwin();
}