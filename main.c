#include "./heads/data.h"
#include "./heads/session.h"

#define ADDR "127.0.0.1"
#define PORT 5000
#define MODE SOCK_STREAM

void serveur();
void client();
int isCommand(buffer_t buff);
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

        if (pid == 0) { 

            fermerSocket(&sockEcoute);

            // TODO : Gérer la communication avec le client
            recevoir(&sockEch, buff, NULL);

            if(isCommand(buff))
            {
                command_manager(buff);
            }
            else
            {
                printf("Message reçu : %s\n", buff);
            }




            // Fermer la socket client après la communication
            fermerSocket(&sockEch); 

            // Terminer le processus enfant
            exit(0); 
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

    sockConn = connecterClt2Srv(ADDR, PORT);

    // On affiche la socket
    printf("Socket dialogue créée : %d\n", sockConn.fd);

    while(1)
    {
        // TODO : Gérer la communication avec le serveur

        // On demande d'écrire quelque chose

        printf("Entrez votre message : ");
        fgets(buff, MAX_BUFFER, stdin);

        // On envoie le message
        envoyer(&sockConn, buff, NULL);

        // if(isCommand(buff))
        // {
        //     printf("Commande détectée\n");
        // }
        // else 
        // {
        //     printf("On envoie le message\n");
        //     envoyer(&sockConn, buff, NULL);
        // }
    }

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

// Gère toutes les commandes, que ce soit connect, disconnect etc..
int command_manager(buffer_t buff)
{
    printf("Commande détectée\n");
    if(strcmp(buff, "/connect\n") == 0)
    {
        // TODO : Gérer la connexion
    }
    else if(strcmp(buff, "/disconnect\n") == 0)
    {
        // TODO : Gérer la déconnexion
        printf("Déconnexion\n");
        exit(0);

    }
    else
    {
        printf("Commande inconnue\n");
    }
    return 0;
}