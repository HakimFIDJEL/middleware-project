#include "../heads/session.h"

#define MAX_SOCKETS 10
// Tableau de sockets
socket_t sockets[MAX_SOCKETS];


void initSockets()
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        sockets[i].fd = -1;
    }
    printf("Tableau de sockets initialisé\n");
}

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

void removeSocket(int index)
{
    if (index < MAX_SOCKETS)
    {
        sockets[index].fd = -1;
    }
}

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


void _adr2struct (struct sockaddr_in *addr, char *adrIP, short port) 
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(adrIP);
    memset(addr->sin_zero, 0, 8);
}

socket_t creerSocket (int mode)
{
	socket_t newSocket;
    CHECK(newSocket.fd = socket(AF_INET, mode, 0), "socket()");
    newSocket.mode = mode;

    return newSocket;
}

socket_t creerSocketAdr (int mode, char *adrIP, short port)
{
    socket_t newSocket = creerSocket(mode);
    _adr2struct(&newSocket.addrLoc, adrIP, port);
    CHECK(bind(newSocket.fd, (struct sockaddr *)&newSocket.addrLoc, sizeof(newSocket.addrLoc)), "bind()");

    return newSocket;
}

socket_t creerSocketEcoute (char *adrIP, short port)
{
    socket_t newSocket = creerSocketAdr(SOCK_STREAM, adrIP, port);
    CHECK(listen(newSocket.fd, 5), "listen()");

    return newSocket;
}

socket_t accepterClt (const socket_t sockEcoute)
{
    socket_t newSocket;
    socklen_t len = sizeof(newSocket.addrDst);
    CHECK(newSocket.fd = accept(sockEcoute.fd, (struct sockaddr *)&newSocket.addrDst, &len), "accept()");
    newSocket.mode = sockEcoute.mode;

    return newSocket;
}

socket_t connecterClt2Srv (char *adrIP, short port) 
{
    socket_t newSocket = creerSocket(SOCK_STREAM);
    _adr2struct(&newSocket.addrDst, adrIP, port);
    CHECK(connect(newSocket.fd, (struct sockaddr *)&newSocket.addrDst, sizeof(newSocket.addrDst)), "connect()");

    return newSocket;
}

void fermerSocket (socket_t *sock)
{
    if (close(sock->fd) == -1) {
        perror("Erreur lors de la fermeture de la socket");
    }
    printf("[fermerSocket] Socket %d fermée\n", sock->fd);
}

void zombieManager()
{
    // Gestion des processus zombies
    struct sigaction sa;
    sa.sa_handler = SIG_IGN; // Ignorer le signal SIGCHLD pour éviter les processus zombies
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Erreur lors de la configuration de SIGCHLD");
        exit(EXIT_FAILURE);
    }
}