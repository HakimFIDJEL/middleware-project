/**
 *	\file		main.c
 *	\brief		Programme principal du chat
 *	\author		Hakim FIDJEL & Younes BOUGHRIET
 *	\date		25 mars 2024
 *	\version	1.0
 */

#include "./heads/data.h"
#include "./heads/session.h"
#include "./heads/users.h"
#include "./heads/channels.h"
#include "./heads/messages.h"
#include "./heads/ncurses.h"
#include <signal.h>

#include <locale.h>

#define ADDR "127.0.0.1" // Adresse IP du serveur
#define PORT 5000       // Port du serveur
#define MODE SOCK_STREAM // Mode de connexion 
#define SIG_ARRET SIGINT // Signal d'arrêt

int flag_start_client = 1;

volatile bool sessionActive = false;

/**
 * @brief Fonction principale du programme pour le serveur
 * @fn void serveur()
 * @param void
 */
void serveur();

/**
 * @brief Fonction principale du programme pour le client
 * @fn void client()
 * @param void
 */
void client();

/**
 * @brief Fonction qui vérifie si la commande est une commande
 * @fn int is_command(buffer_t buff)
 * @param buff : la commande ou le message
 * @return int
 */
int is_command(buffer_t buff);

/**
 * @brief Fonction qui gère le dialogue entre le serveur et le client
 * @fn void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
 * @param sockEch : la socket de dialogue
 * @param buff : le buffer
 * @param serial : la fonction de sérialisation
 */
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial);

/**
 * @brief Fonction qui gère l'envoi des messages du client
 * @fn void *EnvoiClt(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *EnvoiClt(void *arg);

/**
 * @brief Fonction qui gère la réception des messages du client
 * @fn void *ReceptionClt(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *ReceptionClt(void *arg);

/**
 * @brief Fonction qui gère les threads pour les clients
 * @fn void *server_thread(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *server_thread(void *arg);

/**
 * @brief Fonction qui gère les commandes
 * @fn int command_manager(buffer_t buff, User *user)
 * @param buff : la commande
 * @param user : l'utilisateur
 * @return int
 */
int command_manager(buffer_t buff, User *user);

/**
 * @brief Fonction qui gère la déconnexion
 * @fn int disconnect(buffer_t buff)
 * @param buff : la commande
 * @return int
 */
int disconnect(buffer_t buff);

/**
 * @brief Fonction qui gère les signaux
 * @fn void signal_handler(int sig, siginfo_t *siginfo, void *context)
 * @param sig : le signal
 * @param siginfo : les informations du signal
 * @param context : le contexte
 */
void signal_handler(int sig, siginfo_t *siginfo, void *context);

extern WINDOW *top_win; // Fenêtre du haut
extern WINDOW *bottom_win; // Fenêtre du bas
extern WINDOW *logs_win; // Fenêtre des logs

extern int height, width, main_width, side_width;

/**
 * @brief La fonction principale du programme qui lance le serveur ou le client
 * @fn int main()
 * @param void
 * @return int
 */
int main()
{
#ifdef SERVEUR
    serveur();
#else
    client();
#endif
    return 0;
}

socket_t sockEcoute; // Socket d'écoute

/**
 * @brief Fonction principale du programme pour le serveur
 * @fn void serveur()
 * @param void
 */
void serveur()
{
    system("clear");
    socket_t sockEch; // Socket de dialogue
    buffer_t buff;   // Buffer
    pid_t pid;      // PID du processus

    init_users();
    init_channels();

    // initSockets();

    struct sigaction act; // Structure pour les signaux
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &signal_handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIG_ARRET, &act, NULL);

    // On crée le channel par défaut (le lobby)
    strcpy(buff, "Server");
    User *host = add_user(sockEcoute, 0, buff);
    buff[0] = '\0';

    strcpy(buff, "Lobby");
    Channel lobby = add_channel(*host, buff);
    buff[0] = '\0';

    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);

    // Gestion des processus zombies
    zombieManager();

    init_messages(get_channels());

    while (1)
    {
        // Accepter une connexion
        sockEch = accepterClt(sockEcoute);
        printf("Socket dialogue créée : %d\n", sockEch.fd);

        // On crée un thread pour chaque client
        pthread_t server;
        pthread_create(&server, NULL, &server_thread, &sockEch);
    }
}

/**
 * @brief Fonction qui gère les threads pour les clients
 * @fn void *server_thread(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *server_thread(void *arg)
{
    socket_t sockEch = *(socket_t *)arg; // Socket de dialogue
    buffer_t buff; // Buffer

    printf("[Server_thread] Nouvelle connexion depuis l'IP %s\n", inet_ntoa(sockEch.addrLoc.sin_addr));

    dialogueSrv(&sockEch, buff, NULL);

    remove_user(get_user_by_socket(sockEch));
    fermerSocket(&sockEch);
    return NULL;
}

/**
 * @brief Fonction principale du programme pour le client
 * @fn void client()
 * @param void
 */
void client()
{
    socket_t sockConn; // Socket de dialogue
    buffer_t buff; // Buffer

    // TODO : Ecran d'accueil qui attend /connect

    // On affiche la socket
    sockConn = connecterClt2Srv(ADDR, PORT);
    // printf("[Client] Socket dialogue créée : %d\n", sockConn.fd); //TODO
    sessionActive = true;

    init_ncurses();
    create_windows();
    config_colors();

    // Activer le défilement pour top_win
    scrollok(top_win, TRUE);

    // Activer le défilement pour logs_win
    scrollok(logs_win, TRUE);

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
    // ecrire au milieu de la fenetre et en plus grand Discord Style Chat (type 'exit' to quit)
    int height, width;
    getmaxyx(top_win, height, width); // Get the dimensions of the window
    mvwprintw(top_win, 2, (width - strlen("Discord Style Chat (type '/disconnect' to quit)")) / 2, "Discord Style Chat (type '/disconnect' to quit)");
    wrefresh(top_win);

    // On fait deux threads pour gérer l'envoi et la réception
    pthread_t threadEnvoi, threadReception;

    // On lance les threads
    pthread_create(&threadReception, NULL, ReceptionClt, &sockConn);
    pthread_create(&threadEnvoi, NULL, EnvoiClt, &sockConn);

    // On attend la fin des threads
    pthread_join(threadEnvoi, NULL);
    pthread_join(threadReception, NULL);

    // On ferme la socket de dialogue
    printf("[Client] Fermeture de la socket dialogue\n");
    fermerSocket(&sockConn);
    cleanup();
}

/**
 * @brief Fonction qui vérifie si la commande est une commande
 * @fn int is_command(buffer_t buff)
 * @param buff : la commande ou le message
 * @return int
 */
int is_command(buffer_t buff)
{
    if (buff[0] == '/')
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Fonction qui gère le dialogue entre le serveur et le client
 * @fn void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
 * @param sockEch : la socket de dialogue
 * @param buff : le buffer
 * @param serial : la fonction de sérialisation
 */
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
{
    User *users = get_users();
    char message[MAX_BUFFER];

    // Recevoir
    recevoir(sockEch, buff, NULL);
    User *user = add_user(*sockEch, 0, buff);

    Channel *lobby = get_channel_by_id(0);
    add_user_to_channel(*user, lobby);

    while (1)
    {
        // Recevoir
        recevoir(sockEch, buff, NULL);
        printf("[dialogueSrv] %s : %s\n", user->name, buff);

        // On stocke le message dans le fichier

        // char message[MAX_BUFFER];
        // strcpy(message, buff);

        if (is_command(buff))
        {
            // Si la commande est /disconnect
            if (disconnect(buff))
            {
                sessionActive = false;
                return;
            }
            command_manager(buff, user);
        }
        else
        {

            store_message(user->currentChannel, user->id, buff);

            // Envoyer

            // On transfert le message à tous les autres clients qui sont dans le même lobby
            for (int i = 0; i < MAX_USERS; i++)
            {

                if (user_exists(users[i].id) &&
                    is_user_in_channel(users[i], *get_channel_by_id(user->currentChannel)) &&
                    users[i].id != user->id &&
                    users[i].id != 0)
                {
                    // On ajoute à buffer le nom de l'utilisateur qui a envoyé le message

                    strcpy(message, "[");
                    strcat(message, user->name);
                    strcat(message, "] ");
                    strcat(message, buff);

                    envoyer(&(users[i].socket), message, NULL);
                }
            }
        }
        // On vide le buffer
        buff[0] = '\0';
        // On vide le message
        message[0] = '\0';
    }
    return;
}

/**
 * @brief Fonction qui gère l'envoi des messages du client
 * @fn void *EnvoiClt(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *EnvoiClt(void *arg)
{
    socket_t *sockConn = (socket_t *)arg;
    char buff[MAX_BUFFER];
    char message[MAX_BUFFER];
    int ch, i = 0;

    int flag_pseudo = 1;

    while (sessionActive)
    {

        //Ecrire Entrez votre pseudo
        if (flag_pseudo)
        {
            wmove(top_win, 1, 1);
            wprintw(top_win, "Entrez votre pseudo : ");
            wrefresh(top_win);
        }


        wmove(bottom_win, 1, 1); // Move cursor to start of input line
        wclrtoeol(bottom_win);   // Clear the input line
        box(bottom_win, 0, 0);   // Redraw box to ensure borders are intact
        wrefresh(bottom_win);    // Refresh the window to reflect changes

        while (1)
        {
            ch = wgetch(bottom_win);
            if (ch == '\n')
            {
                buff[i] = '\0';
                i = 0;
                break;
            }
            if (ch == KEY_RESIZE)
            {
                endwin();
                refresh();
                clear();

                getmaxyx(stdscr, height, width);
                main_width = width * 3 / 4;
                side_width = width - main_width;

                delwin(top_win);
                delwin(bottom_win);
                delwin(logs_win);

                create_windows();
                config_colors();

                box(top_win, 0, 0);
                box(bottom_win, 0, 0);
                box(logs_win, 0, 0);
                wrefresh(top_win);
                wrefresh(bottom_win);
                wrefresh(logs_win);
                continue;
            }
            if (ch == KEY_BACKSPACE || ch == 127)
            {
                if (i > 0)
                {
                    i--;
                    mvwdelch(bottom_win, 1, i + 1);
                }
            }
            else if (i < MAX_BUFFER - 1)
            {
                buff[i++] = ch;
                waddch(bottom_win, ch);
            }
        }

        if (is_command(buff))
        {
            envoyer(sockConn, buff, NULL);
            if (disconnect(buff))
            {
                sessionActive = false;
                // On ferme la session et on coupe le code
                system("clear");
                exit(0);

                return NULL;
            }
            // other command handling
        }
        else
        {
            if (flag_pseudo)
            {
                char start_line[1024] = "Hello ";
                strcat(start_line, buff);
                display_message(top_win, start_line, 1);
                flag_pseudo = 0;
            }
            else
            {

                // si message vide, on ne l'envoie pas
                if (strlen(buff) == 0)
                {
                    continue;
                }

                strcpy(message, "[You] ");
                strcat(message, buff);
                display_message(top_win, message, 0);
            }
            envoyer(sockConn, buff, NULL);
        }

        buff[0] = '\0'; // Clear buffer after sending
    }

    return NULL;
}

/**
 * @brief Fonction qui gère la réception des messages du client
 * @fn void *ReceptionClt(void *arg)
 * @param arg : la socket de dialogue
 * @return void *
 */
void *ReceptionClt(void *arg)
{
    socket_t *sockConn = (socket_t *)arg;
    buffer_t buff;

    while (sessionActive)
    {
        // Recevoir
        recevoir(sockConn, buff, NULL);

        // Si le buff commence par "[Server]"
        if (strncmp(buff, "[Server]", 8) == 0 || strncmp(buff, "[command_manager]", 16) == 0)
        {

            display_message(logs_win, buff, 1);
            // Mettre le curseur en bas dans bottom_win
            wmove(bottom_win, 1, 1);
            wrefresh(bottom_win);
        }
        else
        {
            display_message(top_win, buff, 0);
            wmove(bottom_win, 1, 1);
            wrefresh(bottom_win);
        }

        buff[0] = '\0'; // Nettoyage du buffer après traitement
    }

    return NULL;
}

/**
 * @brief Fonction qui gère les commandes
 * @fn int command_manager(buffer_t buff, User *user)
 * @param buff : la commande
 * @param user : l'utilisateur
 * @return int
 */
int command_manager(buffer_t buff, User *user)
{
    char retour[1024];
    int id;
    int count;

    Channel channel;
    Channel *channel_pointer;

    char *args[10];
    int argc = 0; // Compteur d'arguments

    // Utiliser strtok pour diviser la chaîne en tokens basés sur l'espace
    char *token = strtok(buff, " ");
    while (token != NULL && argc < 10 - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;

    // Exemple de traitement : afficher les arguments
    for (int i = 0; i < argc; i++)
    {
        // printf("Argument %d: %s\n", i, args[i]);
    }

    // printf("Nombre d'arguments : %d\n", argc);

    // printf("Commande : -%c-\n", args[0][1]);

    // Ici, vous pouvez traiter les arguments comme vous le souhaitez
    switch (args[0][1])
    {

    // Création d'un channel
    case 'g':
        if (argc != 2)
        {
            strcpy(retour, "[Server] Erreur : /g <nom du channel>\n");
        }
        else
        {
            // On crée un channel
            Channel channel = add_channel(*user, args[1]);
            strcpy(retour, "[Server] Channel créé\n");
            init_messages(get_channels());
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Inviter dans un channel
    case 'i':
        if (argc != 3)
        {
            strcpy(retour, "[Server] Erreur : /i <id du channel> <id de l'utilisateur>\n");
        }
        else
        {
            // On récupère le channel
            channel_pointer = get_channel_by_id(atoi(args[1]));

            // On vérifie si l'utilisateur est host
            if (channel_pointer->host.id == user->id)
            {

                // On récupère l'id de l'utilisateur
                id = atoi(args[2]);

                // On récupère l'utilisateur
                User *invited = get_user_by_id(id);

                // On ajoute l'utilisateur au channel
                add_user_to_channel(*invited, channel_pointer);

                strcpy(retour, "[Server] Utilisateur invité\n");
            }
            else
            {
                strcpy(retour, "[Server] Erreur : Vous n'êtes pas l'hôte du channel\n");
            }
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Joindre un channel
    case 'j':
        if (argc != 2)
        {
            strcpy(retour, "[Server] Erreur : /j <id du channel>\n");
        }
        else
        {
            // On récupère le channel
            channel_pointer = get_channel_by_id(atoi(args[1]));

            // On vérifie si le channel existe
            if (channel_pointer == NULL)
            {
                strcpy(retour, "[Server] Erreur : Ce channel n'existe pas\n");
                envoyer(&(user->socket), retour, NULL);
                break;
            }

            printf("[command_manager] Channel id : %d\n", channel_pointer->id);

            // On vérifie si on est autorisé dans le channel
            if (is_user_allowed_in_channel(*user, *channel_pointer))
            {
                connect_user_to_channel(user, channel_pointer->id);
                strcpy(retour, "[Server] Utilisateur connecté\n");
            }
            else
            {
                strcpy(retour, "[Server] Erreur : Vous n'êtes pas autorisé dans ce channel\n");
            }
        }
        printf("[command_manager] Channel actuel : %d\n", user->currentChannel);
        envoyer(&(user->socket), retour, NULL);
        break;

    // Kick d'un channel
    case 'k':
        if (argc != 3)
        {
            strcpy(retour, "[Server] Erreur : /k <id du channel> <id de l'utilisateur>\n");
        }
        else
        {
            // On récupère le channel
            channel_pointer = get_channel_by_id(atoi(args[1]));

            // On vérifie si le channel existe
            if (channel_pointer == NULL)
            {
                strcpy(retour, "[Server] Erreur : Ce channel n'existe pas\n");
                envoyer(&(user->socket), retour, NULL);
                break;
            }

            // On vérifie si l'utilisateur est host
            if (channel_pointer->host.id == user->id)
            {

                // On récupère l'id de l'utilisateur
                id = atoi(args[2]);

                // On récupère l'utilisateur
                User *invited = get_user_by_id(id);

                // On supprime l'utilisateur du channel
                remove_user_from_channel(*invited, channel_pointer);

                // Si le current Channel de l'utilisateur est le channel qu'on vient de kick, on le ramène au lobby
                if (invited->currentChannel == channel_pointer->id)
                {
                    connect_user_to_channel(invited, 0);
                }

                strcpy(retour, "[Server] Utilisateur kick\n");
            }
            else
            {
                strcpy(retour, "[Server] Erreur : Vous n'êtes pas l'hôte du channel\n");
            }
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Quitter un channel
    case 'q':
        if (argc != 2)
        {
            strcpy(retour, "[Server] Erreur : /q <id du channel>\n");
        }
        else
        {
            // On récupère le channel
            channel_pointer = get_channel_by_id(atoi(args[1]));

            // On vérifie si le channel existe
            if (channel_pointer == NULL)
            {
                strcpy(retour, "[Server] Erreur : Ce channel n'existe pas\n");
                envoyer(&(user->socket), retour, NULL);
                break;
            }

            // On vérifie si le channel n'est pas le lobby
            if (channel_pointer->id == 0)
            {
                strcpy(retour, "[Server] Erreur : Vous ne pouvez pas quitter le lobby\n");
                envoyer(&(user->socket), retour, NULL);
                break;
            }

            // Si l'utilisateur est l'host, il ne peut quitter que si c'est le seul utilisateur dans le channel
            if (channel_pointer->host.id == user->id)
            {
                count = 0;
                for (int i = 0; i < MAX_USERS; i++)
                {
                    if (channel_pointer->users[i] != -1)
                    {
                        count++;
                    }
                }
                if (count > 1)
                {
                    strcpy(retour, "[Server] Erreur : Vous ne pouvez pas quitter le channel, vous êtes l'hôte et il reste des utilisateurs\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }
                else
                {
                    remove_channel(channel_pointer);
                    strcpy(retour, "[Server] Channel supprimé\n");
                }
            }

            // On supprime l'utilisateur du channel
            remove_user_from_channel(*user, channel_pointer);

            // Si le current Channel de l'utilisateur est le channel qu'on vient de quitter, on le ramène au lobby
            if (user->currentChannel == channel_pointer->id)
            {
                connect_user_to_channel(user, 0);
            }

            strcpy(retour, "[Server] Utilisateur retiré\n");
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Détruire un channel
    case 's':
        if (argc != 2)
        {
            strcpy(retour, "[Server] Erreur : /s <nom du channel>\n");
            break;
        }
        else
        {
            // On récupère le channel
            channel_pointer = get_channel_by_id(atoi(args[1]));

            // On vérifie si le channel existe
            if (channel_pointer == NULL)
            {
                strcpy(retour, "[Server] Erreur : Ce channel n'existe pas\n");
                envoyer(&(user->socket), retour, NULL);
                break;
            }

            // On vérifie si l'utilisateur est host
            if (channel_pointer->host.id == user->id)
            {
                count = 0;
                for (int i = 0; i < MAX_USERS; i++)
                {
                    if (channel_pointer->users[i] != -1)
                    {
                        count++;
                    }
                }
                if (count > 1)
                {
                    strcpy(retour, "[Server] Erreur : Vous ne pouvez pas supprimer le channel, il reste des utilisateurs\n");
                }
                else if (user->currentChannel == channel_pointer->id)
                {
                    strcpy(retour, "[Server] Erreur : Vous ne pouvez pas supprimer le channel, vous êtes dedans\n");
                }
                else
                {
                    remove_channel(channel_pointer);
                    strcpy(retour, "[Server] Channel supprimé\n");
                }
            }
            else
            {
                strcpy(retour, "[Server] Erreur : Vous n'êtes pas l'hôte du channel\n");
            }
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Liste des channels
    case 'l':
        if (argc != 1)
        {
            strcpy(retour, "[Server] Erreur : /l\n");
        }
        else
        {
            strcpy(retour, "[Server] Liste des channels :\n");

            display_channels(*user, retour);
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Liste des utilisateurs
    case 'u':
        if (argc != 1)
        {
            strcpy(retour, "[Server] Erreur : /l\n");
        }
        else
        {
            printf("[command_manager] Channel actuel : %d\n", user->currentChannel);
            Channel channel = *get_channel_by_id(user->currentChannel);
            printf("[command_manager] Channel actuel : %d %s\n", channel.id, channel.name);

            strcpy(retour, "[Server] Liste des utilisateurs :\n");
            display_users_in_channel(channel, retour);
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    // Help
    case 'h':
        if (argc != 1)
        {
            strcpy(retour, "[Server] Erreur : /h\n");
        }
        else
        {
            strcpy(retour, "[Server] Commandes disponibles :\n");
            strcat(retour, "/g <nom du channel> : Créer un channel\n");
            strcat(retour, "/i <id du channel> <id de l'utilisateur> : Inviter un utilisateur dans un channel\n");
            strcat(retour, "/j <id du channel> : Joindre un channel\n");
            strcat(retour, "/k <id du channel> <id de l'utilisateur> : Kick un utilisateur d'un channel\n");
            strcat(retour, "/q <id du channel> : Quitter un channel\n");
            strcat(retour, "/s <id du channel> : Supprimer un channel\n");
            strcat(retour, "/l : Liste des channels\n");
            strcat(retour, "/u : Liste des utilisateurs\n");
            strcat(retour, "/h : Aide\n");
        }
        envoyer(&(user->socket), retour, NULL);
        break;

    default:
        printf("[command_manager] Commande inconnue pour le channel\n");
        break;
    }

    // On vide le buffer
    retour[0] = '\0';

    return 0;
}

/**
 * @brief Fonction qui gère la déconnexion
 * @fn int disconnect(buffer_t buff)
 * @param buff : la commande
 * @return int
 */
int disconnect(buffer_t buff)
{
    if (strcmp(buff, "/disconnect") == 0)
    {
        // On ferme la session

        return 1;
    }
    return 0;
}

/**
 * @brief Fonction qui gère les signaux
 * @fn void signal_handler(int sig, siginfo_t *siginfo, void *context)
 * @param sig : le signal
 * @param siginfo : les informations du signal
 * @param context : le contexte
 */
void signal_handler(int sig, siginfo_t *siginfo, void *context)
{

    if (siginfo->si_pid == 0)
    {
        fermerSocket(&sockEcoute);
        printf("\n\n/======= Fermeture du serveur =======/\n\n");
        exit(0);
    }
}
