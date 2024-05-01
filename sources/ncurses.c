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

int height, width, main_width, side_width; // Dimensions de la fenêtre
WINDOW *top_win, *bottom_win, *logs_win;  // Fenêtres

/**
 * @brief Initialize ncurses
 * @fn void init_ncurses()
 * @param void
 * @return void
 */
void init_ncurses()
{
    setlocale(LC_ALL, ""); // Pour supporter les caractères UTF-8
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
 * @brief Create windows
 * @fn void create_windows()
 * @param void
 * @return void
 */
void create_windows()
{
    top_win = newwin(height - 3, main_width, 0, 0);
    bottom_win = newwin(3, main_width, height - 3, 0);
    logs_win = newwin(height, side_width, 0, main_width);
}

/**
 * @brief Display the main window
 * @fn void config_colors()
 * @param void
 * @return void
 */
void config_colors()
{
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
 * @brief Display message in the window specified
 * @fn void display_message(WINDOW *win, char *msg, int root)
 * @param win the window to display the message
 * @param msg the message to display
 * @param root 1 if the message is from the server, 0 otherwise
 */
void display_message(WINDOW *win, char *msg, int root)
{
    int y, x, max_y, max_x;
    getyx(win, y, x);            // Récupère la position courante du curseur
    getmaxyx(win, max_y, max_x); // Récupère les dimensions de la fenêtre

    if (y >= max_y - 1)
    {                  // Vérifie si le curseur est à la fin de la fenêtre
        wscrl(win, 1); // Fait défiler la fenêtre d'une ligne vers le haut
    }

    // Affichage du message
    if (root == 1)
    {
        wattron(win, COLOR_PAIR(4));
    }
    else
    {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, y, 1, "%s\n", msg);
    wattroff(win, COLOR_PAIR(root == 1 ? 4 : 2));

    box(win, 0, 0); // Redessine les bordures
    wrefresh(win);  // Rafraîchit la fenêtre
}

/**
 * @brief The main loop of the ncurses interface
 * @fn void main_loop_ncurses()
 * @param void
 */
void main_loop_ncurses()
{
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

    while (1)
    {
        wmove(bottom_win, 1, 1); // Move cursor to start of input line
        wclrtoeol(bottom_win);   // Clear the input line
        box(bottom_win, 0, 0);   // Redraw box to ensure borders are intact
        wrefresh(bottom_win);    // Refresh the window to reflect changes

        i = 0;
        while (1)
        {
            ch = wgetch(bottom_win);
            if (ch == '\n')
                break;
            if (ch == KEY_RESIZE)
            {
                endwin();  // Temporarily end ncurses mode to clear buffers
                refresh(); // Refresh the standard screen to clear the ncurses formatting
                clear();   // Clear the screen from any leftovers

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
            if (ch == KEY_BACKSPACE || ch == 127)
            {
                if (i > 0)
                {
                    i--;
                    mvwdelch(bottom_win, 1, i + 1);
                }
            }
            else if (i < MAX_MSG - 1)
            {
                msg[i++] = ch;
                waddch(bottom_win, ch);
            }
        }
        msg[i] = '\0';

        if (strcmp(msg, "/disconnect") == 0)
            break;

        // Display message in top window
        display_message(top_win, msg, 0);
    }
}

/**
 * @brief Cleanup ncurses
 * @fn void cleanup()
 * @param void
 */
void cleanup()
{
    delwin(top_win);
    delwin(bottom_win);
    delwin(logs_win);
    endwin();
}