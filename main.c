#include "./heads/data.h"
#include "./heads/session.h"
#include "./heads/users.h"
#include "./heads/channels.h"
#include "./heads/display.h"
#include <signal.h>

#define ADDR "127.0.0.1"
#define PORT 5005
#define MODE SOCK_STREAM
#define SIG_ARRET SIGINT

int flag_start_client = 1;

volatile bool sessionActive = false;


void serveur();
void client();
int is_command(buffer_t buff);
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial, User user);
void * EnvoiClt(void * arg);
void * ReceptionClt(void * arg);
void *server_thread(void * arg);
int command_manager(buffer_t buff, User user);
int disconnect(buffer_t buff);

void signal_handler(int sig, siginfo_t *siginfo, void *context);


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
    User host = add_user(sockEcoute, 0, "Server");
    Channel lobby = add_channel(host, "Lobby");

    
    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);

    // Gestion des processus zombies
    zombieManager();


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
    User user = add_user(sockEch, 0, "Client");

    printf("Connexion de %s:%d\n", inet_ntoa(sockEch.addrLoc.sin_addr), ntohs(sockEch.addrLoc.sin_port));

    dialogueSrv(&sockEch, buff, NULL, user);


    remove_user(user);
    fermerSocket(&sockEch);
    return NULL;
}

Client my_client = { .message_count = 0 };

void client()
{
    system("clear");
    printf("Enter your username: ");
    fgets(my_client.username, 100, stdin);
    my_client.username[strlen(my_client.username) - 1] = '\0';

    system("clear");

    socket_t sockConn;
    buffer_t buff;


    // TODO : Ecran d'accueil qui attend /connect

    // On affiche la socket
    sockConn = connecterClt2Srv(ADDR, PORT);
    printf("Socket dialogue créée : %d\n", sockConn.fd);
    

    sessionActive = true;
    


    // On fait deux threads pour gérer l'envoi et la réception
    pthread_t threadEnvoi, threadReception;

    // On lance les threads
    pthread_create(&threadReception, NULL, ReceptionClt, &sockConn);
    pthread_create(&threadEnvoi, NULL, EnvoiClt, &sockConn);

    // On attend la fin des threads
    pthread_join(threadEnvoi, NULL);
    pthread_join(threadReception, NULL);

    
    // On ferme la socket de dialogue
    printf("Fermeture de la socket dialogue\n");
    fermerSocket(&sockConn);
}


int is_command(buffer_t buff)
{
    if(buff[0] == '/')
    {
        return 1;
    }
    return 0;
}



void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial, User user)
{
    User *users = get_users(); 


    while(1)
    {
        // Recevoir
        recevoir(sockEch, buff, NULL);
        printf("\n /== User[%d] : %s\n",user.id, buff);
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
                if(user_exists(users[i].id) && is_user_in_channel(users[i], get_channel_by_id(user.currentChannel)) && users[i].id != user.id && users[i].id != 0)
                {
                    // envoyer(&(users[i].socket), message, NULL);
                    envoyer(&(users[i].socket), buff, NULL);
                }   
            }


        }
        // On vide le buffer
        buff[0] = '\0';
    }

    return;
}

void * EnvoiClt(void * arg)
{
    socket_t *sockConn = (socket_t *) arg;
    buffer_t buff;

    while(1)
    {

        // flag_start_client = print_messages(&my_client, flag_start_client);
        printf("Message à envoyer : ");
        fgets(buff, MAX_BUFFER, stdin);

        // add_message(&my_client, buff);


        // Si la commande est /disconnect
        envoyer(sockConn, buff, NULL);
        


        if(is_command(buff))
        {
            if(disconnect(buff))
            {
                sessionActive = false;
                return NULL;
            }
        }
        // On vide le buffer
        buff[0] = '\0';
    }

    return NULL;
}

void * ReceptionClt(void * arg)
{
    socket_t *sockConn = (socket_t *) arg;
    buffer_t buff;

    while(sessionActive)
    {
        // Recevoir
        recevoir(sockConn, buff, NULL);
        printf("Message reçu : %s\n", buff);


        // On vide le buffer
        buff[0] = '\0';
    }

    return NULL;
}


// Fonction qui gère les commandes, elles peuvent avoir plusieurs arguments et commencent par /
int command_manager(buffer_t buff, User user)
{
    char retour[1024];
    int id;
    Channel channel;
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
        printf("Argument %d: %s\n", i, args[i]);
    }

    printf("Nombre d'arguments : %d\n", argc);

    printf("Commande : -%c-\n", args[0][1]);
   
   

    // Ici, vous pouvez traiter les arguments comme vous le souhaitez
    switch(args[0][1])
    {


        // Création d'un channel
        case 'g':

            if (argc != 2)
            {
                strcpy(retour, "Erreur : /g <nom du channel>\n");
                break;
            }
            

            channel = add_channel(user, args[1]);

            strcpy(retour, "Création d'un channel : ");
            strcat(retour, channel.name);
            printf("Création d'un channel : %s\n", channel.name);
            
            envoyer(&(user.socket), retour, NULL);
        break;

        // Inviter dans un channel
        case 'i':

        break;

        // Joindre un channel
        case 'j':

        break;

        // Kick d'un channel
        case 'k':

        break;

        // Quitter un channel
        case 'q':

        break;

        // Détruire un channel
        case 's':
            if(argc != 2)
            {
                strcpy(retour, "Erreur : /s <nom du channel>\n");
                break;
            }
            // On récupère le channel
            id = atoi(args[1]);
            channel = get_channel_by_id(id);
            if(channel.id == -1 && channel.host.id != user.id)
            {
                strcpy(retour, "Erreur : Channel introuvable\n");
                break;
            }
            remove_channel(channel);
            strcpy(retour, "Channel supprimé\n");
            

            envoyer(&(user.socket), retour, NULL);
        break;

        // Liste des channels
        case 'l':
            if(argc != 1)
            {
                strcpy(retour, "Erreur : /l\n");
                break;
            }
            display_channels();
        break;

        // Liste des utilisateurs
        case 'u':

        break;

        // Help
        case 'h':
            // Affiche la liste des commandes
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
    if(strcmp(buff, "/disconnect\n") == 0)
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
