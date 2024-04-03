#include "./heads/data.h"
#include "./heads/session.h"

#define ADDR "127.0.0.1"
#define PORT 5000
#define MODE SOCK_STREAM

void serveur();
void client();
int isCommand(buffer_t buff);
void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial);
void dialogueClt(socket_t *sockConn, buffer_t buff, pFct deSerial);

int command_manager(buffer_t buff);



int main () {
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
    
    sockEcoute = creerSocketEcoute(ADDR, PORT);
    printf("Socket écoute créée : %d\n", sockEcoute.fd);

    while(1)
    {
        sockEch = accepterClt(sockEcoute);

        // Affiche un message de connexion
        printf("Connexion de %s:%d\n", inet_ntoa(sockEch.addrLoc.sin_addr), ntohs(sockEch.addrLoc.sin_port));

        pid = fork();
        if (pid == -1) {
            perror("Erreur lors du fork");
            fermerSocket(&sockEch);
            continue;
        }

        if (pid == 0) 
        { 
            // Le processus fils n'a pas besoin de cette socket d'écoute
            fermerSocket(&sockEcoute);

            // TODO : Gérer la communication avec le client
            dialogueSrv(&sockEch, buff, NULL);


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

   
    // TODO : Gérer la communication avec le serveur

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

}

void dialogueSrv(socket_t *sockEch, buffer_t buff, pFct serial)
{
    while(1)
    {
        
        // Recevoir
        /**
        recevoir(&sockEch, buff, NULL);
        printf("Message reçu : %s\n", buff);
        **/

        // Envoyer
        /**
        strcpy(buff, "Message à envoyer");
        envoyer(&sockEch, buff, NULL);
        **/

    }

    return;
}

void dialogueClt(socket_t *sockConn, buffer_t buff, pFct deSerial)
{
    while(1)
    {
        // Envoyer
        /**
        strcpy(buff, "Message à envoyer");
        envoyer(&sockConn, buff, NULL);
        **/

        // Recevoir
        /**
        recevoir(&sockConn, buff, NULL);
        printf("Message reçu : %s\n", buff);
        **/

    }

    return;
}