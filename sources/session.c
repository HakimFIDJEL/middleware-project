/**
 *	\file		session.c
 *	\brief		Spécification de la couche Session
 *	\author		Hakim FIDJEL
 *	\date		25 mars 2024
 *	\version	1.0
 */

#include "../heads/session.h"

#define MAX_SOCKETS 10 // Nombre maximal de sockets
// Tableau de sockets
socket_t sockets[MAX_SOCKETS]; // Tableau de sockets

/**
 *	\fn			void initSockets()
 *	\brief		Initialisation du tableau de sockets
 *	\note		Cette fonction permet d'initialiser le tableau des sockets connectés
 *	\result		void
 */
void initSockets()
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        sockets[i].fd = -1;
    }
    printf("Tableau de sockets initialisé\n");
}

/**
 *	\fn			void addSocket(socket_t sock)
 *	\brief		Ajout d'une socket au tableau
 *	\param		sock : la socket à ajouter
 *	\note		Cette fonction permet d'ajouter une socket au tableau des sockets connectés
 *	\result		void
 */
void addSocket(socket_t sock)
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        if (sockets[i].fd == -1)
        {
            sockets[i] = sock;
            break;
        }
    }
}

/**
 *	\fn			void removeSocket(int index)
 *	\brief		Suppression d'une socket du tableau
 *	\param		index : l'indice de la socket à supprimer
 *	\note		Cette fonction permet de supprimer une socket du tableau des sockets connectés
 *	\result		void
 */
void removeSocket(int index)
{
    if (index < MAX_SOCKETS)
    {
        sockets[index].fd = -1;
    }
}

/**
 *	\fn			void displaySockets()
 *	\brief		Affichage des sockets connectés
 *	\note		Cette fonction permet d'afficher les sockets connectés
 *	\result		void
 */
void displaySockets()
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        if (sockets[i].fd != -1)
        {
            printf("Socket %d : %d\n", i, sockets[i].fd);
        }
    }
}

/**
 *	\fn			void _adr2struct (struct sockaddr_in *addr, char *adrIP, short port)
 *	\brief		Transformer une adresse au format humain en structure SocketBSD
 *	\param		addr : structure d'adressage BSD d'une socket INET
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\note		Le domaine dépend du mode choisi (TCP/UDP)
 *	\result		paramètre *adr modifié
 */
void _adr2struct(struct sockaddr_in *addr, char *adrIP, short port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(adrIP);
    memset(addr->sin_zero, 0, 8);
}

/**
 *	\fn			socket_t creerSocket (int mode)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : mode connecté (STREAM) ou non (DGRAM)
 *	\result		socket créée selon le mode choisi
 */
socket_t creerSocket(int mode)
{
    socket_t newSocket;
    CHECK(newSocket.fd = socket(AF_INET, mode, 0), "socket()");
    newSocket.mode = mode;

    return newSocket;
}

/**
 *	\fn			socket_t creerSocketAdr (int mode, char *adrIP, short port)
 *	\brief		Création d'une socket de type DGRAM/STREAM
 *	\param		mode : adresse IP de la socket créée
 *	\param		adrIP : adresse IP de la socket créée
 *	\param		port : port de la socket créée
 *	\result		socket créée dans le domaine choisi avec l'adressage fourni
 */
socket_t creerSocketAdr(int mode, char *adrIP, short port)
{
    socket_t newSocket = creerSocket(mode);
    _adr2struct(&newSocket.addrLoc, adrIP, port);
    CHECK(bind(newSocket.fd, (struct sockaddr *)&newSocket.addrLoc, sizeof(newSocket.addrLoc)), "bind()");

    return newSocket;
}

/**
 *	\fn			creerSocketEcoute (char *adrIP, short port)
 *	\brief		Création d'une socket d'écoute avec l'adressage fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à mettre en écoute
 *	\param		port : port TCP du serveur à mettre en écoute
 *	\result		socket créée avec l'adressage fourni en paramètre et dans un état d'écoute
 *	\note		Le domaine est nécessairement STREAM
 */
socket_t creerSocketEcoute(char *adrIP, short port)
{
    socket_t newSocket = creerSocketAdr(SOCK_STREAM, adrIP, port);
    CHECK(listen(newSocket.fd, 5), "listen()");

    return newSocket;
}

/**
 *	\fn			socket_t accepterClt (const socket_t sockEcoute)
 *	\brief		Acceptation d'une demande de connexion d'un client
 *	\param		sockEcoute : socket d'écoute pour réception de la demande
 *	\result		socket (dialogue) connectée par le serveur avec un client
 */
socket_t accepterClt(const socket_t sockEcoute)
{
    socket_t newSocket;
    socklen_t len = sizeof(newSocket.addrDst);
    CHECK(newSocket.fd = accept(sockEcoute.fd, (struct sockaddr *)&newSocket.addrDst, &len), "accept()");
    newSocket.mode = sockEcoute.mode;

    return newSocket;
}

/**
 *	\fn			socket_t connecterClt2Srv (char *adrIP, short port)
 *	\brief		Crétaion d'une socket d'appel et connexion au seveur dont
 *				l'adressage est fourni en paramètre
 *	\param		adrIP : adresse IP du serveur à connecter
 *	\param		port : port TCP du serveur à connecter
 *	\result		socket connectée au serveur fourni en paramètre
 */
socket_t connecterClt2Srv(char *adrIP, short port)
{
    socket_t newSocket = creerSocket(SOCK_STREAM);
    _adr2struct(&newSocket.addrDst, adrIP, port);
    CHECK(connect(newSocket.fd, (struct sockaddr *)&newSocket.addrDst, sizeof(newSocket.addrDst)), "connect()");

    return newSocket;
}

/**
 *	\fn	        void fermerSocket (socket_t *sock)
 *	\brief		Fermeture d'une socket
 *	\param		sock : socket à fermer
 *   \result		la socket est fermée
 */
void fermerSocket(socket_t *sock)
{
    if (close(sock->fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket");
    }
    printf("[fermerSocket] Socket %d fermée\n", sock->fd);
}

/**
 *	\fn			void zombieManager()
 *	\brief		Le gestionnaire de zombies
 *	\note		Cette fonction permet de gérer les processus zombies
 *	\result		void
 */
void zombieManager()
{
    // Gestion des processus zombies
    struct sigaction sa;
    sa.sa_handler = SIG_IGN; // Ignorer le signal SIGCHLD pour éviter les processus zombies
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("Erreur lors de la configuration de SIGCHLD");
        exit(EXIT_FAILURE);
    }
}