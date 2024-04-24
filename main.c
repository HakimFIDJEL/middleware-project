#include "./heads/data.h"
#include "./heads/session.h"
#include "./heads/users.h"
#include "./heads/channels.h"
#include "./heads/display.h"
#include "./heads/messages.h"
#include "./heads/ncurses.h"
#include <signal.h>

#include <locale.h>


#define ADDR "127.0.0.1"
#define PORT 5002
#define MODE SOCK_STREAM
#define SIG_ARRET SIGINT

int flag_start_client = 1;

volatile bool sessionActive = false;


void serveur();
void client();
int is_command(buffer_t buff);
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial);
void * EnvoiClt(void * arg);
void * ReceptionClt(void * arg);
void *server_thread(void * arg);
int command_manager(buffer_t buff, User* user);
int disconnect(buffer_t buff);

void signal_handler(int sig, siginfo_t *siginfo, void *context);

extern WINDOW *top_win;
extern WINDOW *bottom_win;
extern WINDOW *logs_win;

extern int height, width, main_width, side_width;


int main () 
{
#ifdef SERVEUR
    serveur();
#else
    client();
#endif
    return 0;
}

socket_t sockEcoute;


void serveur()
{



    system("clear");
    socket_t sockEch;
    buffer_t buff;
    pid_t pid;

    init_users();
    init_channels();
    
    // initSockets();


    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &signal_handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIG_ARRET, &act, NULL);


    // On crée le channel par défaut (le lobby)
    strcpy(buff, "Server");
    User* host = add_user(sockEcoute, 0, buff);
    buff[0] = '\0';

    strcpy(buff, "Lobby");
    Channel lobby = add_channel(*host, buff);
    buff[0] = '\0';
    
    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);


    // Gestion des processus zombies
    zombieManager();


    init_messages(get_channels());


    while(1)
    {
        // Accepter une connexion
        sockEch = accepterClt(sockEcoute);
        printf("Socket dialogue créée : %d\n", sockEch.fd);
        
        // On crée un thread pour chaque client
        pthread_t server;
        pthread_create(&server, NULL, &server_thread, &sockEch);
    }

}


void *server_thread(void * arg)
{
    socket_t sockEch = *(socket_t *) arg;
    buffer_t buff;
    

    printf("[Server_thread] Nouvelle connexion depuis l'IP %s\n", inet_ntoa(sockEch.addrLoc.sin_addr));


    dialogueSrv(&sockEch, buff, NULL);

    remove_user(get_user_by_socket(sockEch));
    fermerSocket(&sockEch);
    return NULL;
}


void client()
{
    socket_t sockConn;
    buffer_t buff;

    // TODO : Ecran d'accueil qui attend /connect

    // On affiche la socket
    sockConn = connecterClt2Srv(ADDR, PORT);
    //printf("[Client] Socket dialogue créée : %d\n", sockConn.fd); //TODO
    sessionActive = true;

    init_ncurses();
    create_windows();
    config_colors();

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
    //ecrire au milieu de la fenetre et en plus grand Discord Style Chat (type 'exit' to quit)
    int height, width;
    getmaxyx(top_win, height, width); // Get the dimensions of the window
    mvwprintw(top_win, 2, (width - strlen("Discord Style Chat (type 'exit' to quit)")) / 2, "Discord Style Chat (type 'exit' to quit)");
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

int is_command(buffer_t buff)
{
    if(buff[0] == '/')
    {
        return 1;
    }
    return 0;
}



void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
{
    User *users = get_users(); 

    // Recevoir
    recevoir(sockEch, buff, NULL);
    User* user = add_user(*sockEch, 0, buff);

    Channel *lobby = get_channel_by_id(0);
    add_user_to_channel(*user, lobby);
    

    while(1)
    {
        // Recevoir
        recevoir(sockEch, buff, NULL);
        printf("[dialogueSrv] %s : %s\n",user->name, buff);

        // On stocke le message dans le fichier
        store_message(user->currentChannel, user->id, buff);


        // char message[MAX_BUFFER];
        // strcpy(message, buff);
        

        if(is_command(buff))
        {
            // Si la commande est /disconnect   
            if(disconnect(buff))
            {
                sessionActive = false;
                return;
            }
            command_manager(buff, user);
            
            
        }
        else 
        {
            // Envoyer

            // On répond à celui qui nous a envoyer un message
            //strcpy(buff, "Message à envoyer");
            //envoyer(sockEch, buff, NULL);


            // On transfert le message à tous les autres clients qui sont dans le même lobby

            for (int i = 0; i < MAX_USERS; i++)
            {
                // if(user_exists(users[i].id))
                // {
                //     printf("[dialogueSrv] %d %s : %d\n", users[i].id, users[i].name, users[i].currentChannel);
                // }
                // if(user_exists(users[i].id) && is_user_in_channel(users[i], *get_channel_by_id(user.currentChannel)) && users[i].id != user.id && users[i].id != 0)
                // {
                //     // envoyer(&(users[i].socket), message, NULL);
                //     envoyer(&(users[i].socket), buff, NULL);
                // }   

                if(user_exists(users[i].id) && 
                    is_user_in_channel(users[i], *get_channel_by_id(user->currentChannel)) &&
                    users[i].id != user->id &&
                    users[i].id != 0
                )
                {
                    envoyer(&(users[i].socket), buff, NULL);
                }
            }


        }
        // On vide le buffer
        buff[0] = '\0';
    }
    return;
}



void* EnvoiClt(void* arg) {
    socket_t *sockConn = (socket_t *) arg;
    char buff[MAX_BUFFER];
    int ch, i = 0;

    int flag_pseudo = 1;

    //Pseudo

    wmove(bottom_win, 1, 1);
    wclrtoeol(bottom_win);
    box(bottom_win, 0, 0);
    wrefresh(bottom_win);

    display_message(top_win, "Enter your pseudo", "Server", 1);
    

    while(sessionActive) {
        wmove(bottom_win, 1, 1); // Move cursor to start of input line
        wclrtoeol(bottom_win); // Clear the input line
        box(bottom_win, 0, 0); // Redraw box to ensure borders are intact
        wrefresh(bottom_win); // Refresh the window to reflect changes

        while (1) {
            ch = wgetch(bottom_win);
            if (ch == '\n') break;
            if (ch == KEY_RESIZE) {
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
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (i > 0) {
                    i--;
                    mvwdelch(bottom_win, 1, i + 1);
                }
            } else if (i < MAX_BUFFER - 1) {
                buff[i++] = ch;
                waddch(bottom_win, ch);
            }
        }
        buff[i] = '\0';

        if(is_command(buff)) {
            if(disconnect(buff)) {
                sessionActive = false;
                return NULL;
            }
        }

        envoyer(sockConn, buff, NULL);

        if(flag_pseudo)
        {
            char start_line[1024] = "Hello ";
            strcat(start_line, buff);
            display_message(top_win, start_line, "Server", 1);
            flag_pseudo = 0;
        }
        else 
        {
            display_message(top_win, buff, "You", 0);   
        }
        
        i = 0;
    }

    return NULL;
}

void* ReceptionClt(void* arg) {
    socket_t *sockConn = (socket_t *) arg;
    buffer_t buff;

    while(sessionActive) {
        // Recevoir
        recevoir(sockConn, buff, NULL);

        display_message(top_win, buff, "Message received", 0);

        // On vide le buffer
        buff[0] = '\0';
        wmove(bottom_win, 1, 1); // Move cursor to start of input line
        wclrtoeol(bottom_win); // Clear the input line
        box(bottom_win, 0, 0); // Redraw box to ensure borders are intact
        wrefresh(bottom_win); // Refresh the window to reflect changes

    }

    return NULL;
}



// Fonction qui gère les commandes, elles peuvent avoir plusieurs arguments et commencent par /
int command_manager(buffer_t buff, User* user)
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
    switch(args[0][1])
    {


        // Création d'un channel
        case 'g':
            if (argc != 2)
            {
                strcpy(retour, "[command_manager] Erreur : /g <nom du channel>\n");
            }
            else 
            {
                // On crée un channel
                Channel channel = add_channel(*user, args[1]);
                strcpy(retour, "[command_manager] Channel créé\n");
                init_messages(get_channels());
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Inviter dans un channel
        case 'i':
            if(argc != 3)
            {
                strcpy(retour, "Erreur : /i <id du channel> <id de l'utilisateur>\n");
            }
            else 
            {
                // On récupère le channel
                channel_pointer = get_channel_by_id(atoi(args[1]));

                // On vérifie si l'utilisateur est host
                if(channel_pointer->host.id == user->id)
                {

                    // On récupère l'id de l'utilisateur
                    id = atoi(args[2]);

                    // On récupère l'utilisateur
                    User* invited = get_user_by_id(id);

                    // On ajoute l'utilisateur au channel
                    add_user_to_channel(*invited, channel_pointer);

                    strcpy(retour, "Utilisateur invité\n");
                }
                else 
                {
                    strcpy(retour, "Erreur : Vous n'êtes pas l'hôte du channel\n");
                }
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Joindre un channel
        case 'j':
            if(argc != 2)
            {
                strcpy(retour, "Erreur : /j <id du channel>\n");
            }
            else 
            {
                // On récupère le channel
                channel_pointer = get_channel_by_id(atoi(args[1]));

                // On vérifie si le channel existe
                if(channel_pointer == NULL)
                {
                    strcpy(retour, "Erreur : Ce channel n'existe pas\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }

                printf("[command_manager] Channel id : %d\n", channel_pointer->id);

                // On vérifie si on est autorisé dans le channel
                if(is_user_allowed_in_channel(*user, *channel_pointer))
                {
                    connect_user_to_channel(user, channel_pointer->id);
                    strcpy(retour, "Utilisateur connecté\n");
                }
                else 
                {
                    strcpy(retour, "Erreur : Vous n'êtes pas autorisé dans ce channel\n");
                }
            }
            printf("[command_manager] Channel actuel : %d\n", user->currentChannel);
            envoyer(&(user->socket), retour, NULL);
        break;

        // Kick d'un channel
        case 'k':
            if(argc != 3)
            {
                strcpy(retour, "Erreur : /k <id du channel> <id de l'utilisateur>\n");
            }
            else 
            {
                // On récupère le channel
                channel_pointer = get_channel_by_id(atoi(args[1]));

                // On vérifie si le channel existe
                if(channel_pointer == NULL)
                {
                    strcpy(retour, "Erreur : Ce channel n'existe pas\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }

                // On vérifie si l'utilisateur est host
                if(channel_pointer->host.id == user->id)
                {

                    // On récupère l'id de l'utilisateur
                    id = atoi(args[2]);

                    // On récupère l'utilisateur
                    User* invited = get_user_by_id(id);

                    // On supprime l'utilisateur du channel
                    remove_user_from_channel(*invited, channel_pointer);

                    // Si le current Channel de l'utilisateur est le channel qu'on vient de kick, on le ramène au lobby
                    if(invited->currentChannel == channel_pointer->id)
                    {
                        connect_user_to_channel(invited, 0);
                    }

                    strcpy(retour, "Utilisateur kick\n");
                }
                else 
                {
                    strcpy(retour, "Erreur : Vous n'êtes pas l'hôte du channel\n");
                }
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Quitter un channel
        case 'q':
            if(argc != 2)
            {
                strcpy(retour, "Erreur : /q <id du channel>\n");
            }
            else 
            {
                // On récupère le channel
                channel_pointer = get_channel_by_id(atoi(args[1]));

                // On vérifie si le channel existe
                if(channel_pointer == NULL)
                {
                    strcpy(retour, "Erreur : Ce channel n'existe pas\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }

                // On vérifie si le channel n'est pas le lobby
                if(channel_pointer->id == 0)
                {
                    strcpy(retour, "Erreur : Vous ne pouvez pas quitter le lobby\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }

                // Si l'utilisateur est l'host, il ne peut quitter que si c'est le seul utilisateur dans le channel
                if(channel_pointer->host.id == user->id)
                {
                    count = 0;
                    for (int i = 0; i < MAX_USERS; i++)
                    {
                        if(channel_pointer->users[i] != -1)
                        {
                            count++;
                        }
                    }
                    if(count > 1)
                    {
                        strcpy(retour, "Erreur : Vous ne pouvez pas quitter le channel, vous êtes l'hôte et il reste des utilisateurs\n");
                        envoyer(&(user->socket), retour, NULL);
                        break;
                    }
                    else 
                    {
                        remove_channel(channel_pointer);
                        strcpy(retour, "Channel supprimé\n");
                    }
                }

                // On supprime l'utilisateur du channel
                remove_user_from_channel(*user, channel_pointer);

                // Si le current Channel de l'utilisateur est le channel qu'on vient de quitter, on le ramène au lobby
                if(user->currentChannel == channel_pointer->id)
                {
                    connect_user_to_channel(user, 0);
                }

                strcpy(retour, "Utilisateur retiré\n");
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Détruire un channel
        case 's':
            if(argc != 2)
            {
                strcpy(retour, "Erreur : /s <nom du channel>\n");
                break;
            }
            else 
            {
                // On récupère le channel
                channel_pointer = get_channel_by_id(atoi(args[1]));

                // On vérifie si le channel existe
                if(channel_pointer == NULL)
                {
                    strcpy(retour, "Erreur : Ce channel n'existe pas\n");
                    envoyer(&(user->socket), retour, NULL);
                    break;
                }

                // On vérifie si l'utilisateur est host
                if(channel_pointer->host.id == user->id)
                {
                    count = 0;
                    for (int i = 0; i < MAX_USERS; i++)
                    {
                        if(channel_pointer->users[i] != -1)
                        {
                            count++;
                        }
                    }
                    if(count > 1)
                    {
                        strcpy(retour, "Erreur : Vous ne pouvez pas supprimer le channel, il reste des utilisateurs\n");
                    }
                    else if (user->currentChannel == channel_pointer->id)
                    {
                        strcpy(retour, "Erreur : Vous ne pouvez pas supprimer le channel, vous êtes dedans\n");
                    }
                    else 
                    {
                        remove_channel(channel_pointer);
                        strcpy(retour, "Channel supprimé\n");
                    }
                }
                else 
                {
                    strcpy(retour, "Erreur : Vous n'êtes pas l'hôte du channel\n");
                }
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Liste des channels
        case 'l':
            if(argc != 1)
            {
                strcpy(retour, "Erreur : /l\n");
            }
            else 
            {
                display_channels(*user, retour);
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Liste des utilisateurs
        case 'u':
            if(argc != 1)
            {
                strcpy(retour, "Erreur : /l\n");
            }
            else 
            {
                printf("[command_manager] Channel actuel : %d\n", user->currentChannel);
                Channel channel = *get_channel_by_id(user->currentChannel);
                printf("[command_manager] Channel actuel : %d %s\n", channel.id, channel.name);

                display_users_in_channel(channel, retour);
            }
            envoyer(&(user->socket), retour, NULL);
        break;

        // Help
        case 'h':
            if(argc != 1)
            {
                strcpy(retour, "Erreur : /h\n");
            }
            else 
            {
                strcpy(retour, "Commandes disponibles :\n");
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

        default :
            printf("Commande inconnue pour le channel\n");
        break;

        
    }

    // On vide le buffer
    retour[0] = '\0';   
   
    
    return 0;


}

int disconnect(buffer_t buff)
{
    if(strcmp(buff, "/disconnect") == 0)
    {
        return 1;
    }
    return 0;
}


void signal_handler(int sig, siginfo_t *siginfo, void *context) 
{
    
    if(siginfo->si_pid == 0)
    {
        fermerSocket(&sockEcoute);
        printf("\n\n/======= Fermeture du serveur =======/\n\n");
        exit(0);

    }

    
}
