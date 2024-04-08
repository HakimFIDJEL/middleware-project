#include "./heads/data.h"
#include "./heads/session.h"
#include "./heads/users.h"
#include "./heads/channels.h"



#define ADDR "127.0.0.1"
#define PORT 5000
#define MODE SOCK_STREAM

volatile bool sessionActive = false;


void serveur();
void client();
int isCommand(buffer_t buff);
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial, User user);
void dialogueClt(socket_t *sockConn, buffer_t buff, pFct deSerial);
void * EnvoiClt(void * arg);
void * ReceptionClt(void * arg);

int command_manager(buffer_t buff);



int main () 
{
#ifdef SERVEUR
    serveur();
#else
    client();
#endif
    return 0;
}

void serveur()
{
    socket_t sockEcoute;
    socket_t sockEch;
    buffer_t buff;
    pid_t pid;

    init_users();
    init_channels();
    // initSockets();


    // On crée le channel par défaut (le lobby)
    User host = add_user(sockEcoute, 0);
    Channel lobby = add_channel(0, host, "Lobby");
    connect_user_to_channel(host, lobby.id);

    
    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);

    // Gestion des processus zombies
    zombieManager();


    while(1)
    {
        // Accepter une connexion
        sockEch = accepterClt(sockEcoute);
        printf("Socket dialogue créée : %d\n", sockEch.fd);
        
        User user = add_user(sockEch, lobby.id);
       


        // display_users();



        pid = fork();
        if (pid == -1) {
            perror("Erreur lors du fork");
            fermerSocket(&sockEch);
            continue;
        }
        if (pid == 0) 
        { 
            printf("Connexion de %s:%d\n", inet_ntoa(sockEch.addrLoc.sin_addr), ntohs(sockEch.addrLoc.sin_port));

            // Le processus fils n'a pas besoin de cette socket d'écoute
            fermerSocket(&sockEcoute);




            // displaySockets();


            // TODO : Gérer la communication avec le client
            dialogueSrv(&sockEch, buff, NULL, user);



            // removeSocket(sockEch.fd);

            remove_user(user);
            
            // printf("Déconnexion de %s:%d\n", inet_ntoa(sockEch.addrLoc.sin_addr), ntohs(sockEch.addrLoc.sin_port));            
            printf("Fermeture de la socket dialogue\n");
            exit(0);
            fermerSocket(&sockEch);


        } else {
            // Le processus parent n'a pas besoin de cette socket client
            fermerSocket(&sockEch);
        }

    }

    // Fermer la socket d'écoute
    fermerSocket(&sockEcoute);
    return;
}


void client()
{
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

    // On gère la communication
    // dialogueClt(&sockConn, buff, NULL);

    
    // On ferme la socket de dialogue
    printf("Fermeture de la socket dialogue\n");
    fermerSocket(&sockConn);
}


int isCommand(buffer_t buff)
{
    if(buff[0] == '/')
    {
        return 1;
    }
    return 0;
}

int command_manager(buffer_t buff)
{
    if(strcmp(buff, "/disconnect\n") == 0)
    {
        return 1;
    }
}

void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial, User user)
{
    // User *users = get_users(); 


    while(1)
    {
        // Recevoir
        recevoir(sockEch, buff, NULL);
        printf("\n /== User[%d] : %s\n",user.id, buff);
        char message[MAX_BUFFER];
        strcpy(message, buff);
        

        // Si la commande est /disconnect
        if(isCommand(buff))
        {
            if(command_manager(buff))
            {
                return;
            }
        }
        // Envoyer

        // On répond à celui qui nous a envoyer un message
        strcpy(buff, "Message à envoyer");
        envoyer(sockEch, buff, NULL);


        // On transfert le message à tous les autres clients qui sont dans le même lobby
       

        display_users();

        // for (int i = 0; i < MAX_USERS; i++)
        // {
        //     printf("User id : %d\n", users[i].id);
            // if(user_exists(users[i].id))
            // {

            //     printf("-- User in channel : %d\n", is_user_in_channel(users[i], get_channel_by_id(user.currentChannel)));
            //     printf("-- User id : %d\n", users[i].id);


            //     if(user_exists(users[i].id) && is_user_in_channel(users[i], get_channel_by_id(user.currentChannel)) && users[i].id != user.id && users[i].id != 0)
            //     {
            //         // envoyer(&(users[i].socket), message, NULL);
            //         printf("Socket descripteur : %d\n", users[i].socket.fd);
            //         printf("Message envoyé à %d\n", users[i].id);
            //     }
            // }
        // }




        // On vide le buffer
        memset(buff, 0, MAX_BUFFER);
    }

    return;
}



void * EnvoiClt(void * arg)
{
    socket_t *sockConn = (socket_t *) arg;
    buffer_t buff;

    while(1)
    {
        printf("Entrez votre message : ");
        fgets(buff, MAX_BUFFER, stdin);

        // Si la commande est /disconnect
        if(isCommand(buff))
        {
            // Envoyer
            envoyer(sockConn, buff, NULL);

            if(command_manager(buff))
            {
                sessionActive = false;
                return NULL;
            }
        }
        else 
        {
            // Envoyer
            envoyer(sockConn, buff, NULL);
        }
        // On vide le buffer
        memset(buff, 0, MAX_BUFFER);
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
    }

    return NULL;
}















void dialogueClt(socket_t *sockConn, buffer_t buff, pFct deSerial)
{
    while(1)
    {
        printf("Entrez votre message : ");
        fgets(buff, MAX_BUFFER, stdin);

        // Si la commande est /disconnect
        if(isCommand(buff))
        {
            // Envoyer
            envoyer(sockConn, buff, NULL);

            if(command_manager(buff))
            {
                return;
            }
        }
        else 
        {
            // Envoyer
            envoyer(sockConn, buff, NULL);
        }
        // Recevoir
        recevoir(sockConn, buff, NULL);
        printf("Message reçu : %s\n", buff);

        // On vide le buffer
        memset(buff, 0, MAX_BUFFER);
    }

    return;
}
