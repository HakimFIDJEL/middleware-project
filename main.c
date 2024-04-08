#include "./heads/data.h"
#include "./heads/session.h"
#include "./heads/users.h"
#include "./heads/channels.h"


#define ADDR "127.0.0.1"
#define PORT 5000
#define MODE SOCK_STREAM


void serveur();
void client();
int isCommand(buffer_t buff);
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial);
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

    // init_users();
    // initSockets();
    
    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);

    // Gestion des processus zombies
    zombieManager();


    while(1)
    {
        // Accepter une connexion
        sockEch = accepterClt(sockEcoute);
        printf("Socket dialogue créée : %d\n", sockEch.fd);
       
        // addSocket(sockEch);

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
            dialogueSrv(&sockEch, buff, NULL);



            // removeSocket(sockEch.fd);

            
            printf("Déconnexion de %s:%d\n", inet_ntoa(sockEch.addrLoc.sin_addr), ntohs(sockEch.addrLoc.sin_port));            
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


    // On fait deux threads pour gérer l'envoi et la réception
    // pthread_t threadEnvoi, threadReception;

    // On lance les threads
    // pthread_create(&threadEnvoi, NULL, EnvoiClt, &sockConn);
    // pthread_create(&threadReception, NULL, ReceptionClt, &sockConn);

    // On attend la fin des threads
    // pthread_join(threadEnvoi, NULL);
    // pthread_join(threadReception, NULL);

    // On gère la communication
    dialogueClt(&sockConn, buff, NULL);

    
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

void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
{
    while(1)
    {
        // Recevoir
        recevoir(sockEch, buff, NULL);
        printf("Message reçu : %s\n", buff);
        

        // Si la commande est /disconnect
        if(isCommand(buff))
        {
            if(command_manager(buff))
            {
                return;
            }
        }
        // Envoyer
        strcpy(buff, "Message à envoyer");
        envoyer(sockEch, buff, NULL);
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

    while(1)
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
