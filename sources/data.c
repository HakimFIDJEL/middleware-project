/**
 *	\file		data.c
 *	\brief		Spécification de la couche Data Representation
 *	\author		Hakim FIDJEL
 *	\date		25 mars 2024
 *	\version	1.0
 */
#include "../heads/data.h"


/*
***************************************************************************************************
 *	\noop	   F O N C T I O N S   D E   L A   C O U C H E   R E P R E S E N T A T I O N
 */


/**
 *	\fn			void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...)
 *	\brief		Envoi d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		quoi : requête/réponse à serialiser avant l'envoi
 *	\param 		serial : pointeur sur la fonction de serialisation d'une requête/réponse
 *	\note		si le paramètre serial vaut NULL alors quoi est une chaîne de caractères
 *	\note		Si le mode est DGRAM, l'appel nécessite en plus l'adresse IP et le port.
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...) 
{
    buffer_t content;

    // Serializing
    if (serial != NULL)
        serial(quoi, content);
    else
        strcpy(content, (char *) quoi);

    // Sending
    if (sockEch->mode == SOCK_STREAM)
    {
        _envoyerMessStream(sockEch, content);
    }
    else {
        va_list pArg;

        char * addrIp = va_arg(pArg, char *);
        int port = va_arg(pArg, int);

        va_start(pArg, serial);
            _envoyerMessDGRAM(sockEch, content, addrIp, port);
        va_end(pArg);
    }
}

/**
 *	\fn			void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
 *	\brief		Réception d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		quoi : requête/réponse reçue après dé-serialisation du buffer de réception
 *	\param 		deSerial : pointeur sur la fonction de dé-serialisation d'une requête/réponse
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 *				paramètre sockEch modifié pour le mode DGRAM
 */
void recevoir(socket_t *sockEch, generic quoi, pFct deSerial) {
    buffer_t content;

    // Receiving
    if (sockEch->mode == SOCK_STREAM)
        _recevoirMessStream(sockEch, content);
    else
        _recevoirMessDGRAM(sockEch, content);


    // Deserializing
    if (deSerial != NULL)
        deSerial(content, quoi);
    else
        strcpy(quoi,content);
}


/*
*************************************************************
 *	\noop	   F O N C T I O N S   L O C A L E S
 */

/**
 *	\fn			void _envoyerMessStream(socket_t *sockEch, buffer_t buff)
 *	\brief		Envoi d'un message sur une socket de type STREAM
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		buff : message à envoyer
 *	\result		paramètre sockEch modifié
 */
void _envoyerMessStream(socket_t *sockEch, buffer_t buff)
{
    CHECK(send(sockEch->fd, buff, MAX_BUFFER, 0), "Can't send");
}


/**
 *	\fn			void _envoyerMessDGRAM(socket_t *sockEch, buffer_t buff, char *adrIP, int port)
 *	\brief		Envoi d'un message sur une socket de type DGRAM
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		buff : message à envoyer
 *	\param 		adrIP : adresse IP de destination
 *	\param 		port : port de destination
 *	\result		paramètre sockEch modifié
 */
void _envoyerMessDGRAM(socket_t *sockEch, buffer_t buff, char *adrIP, int port)
{
    struct sockaddr_in addr;
    _adr2struct(&addr, adrIP, port);
    CHECK(sendto(sockEch->fd, buff, MAX_BUFFER, 0, (struct sockaddr *)&addr, sizeof(addr)), "Can't send");
}


/**
 *	\fn			void _recevoirMessStream(socket_t *sockEch, buffer_t buff)
 *	\brief		Réception d'un message sur une socket de type STREAM
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		buff : message reçu
 *	\result		paramètre sockEch modifié
 */
void _recevoirMessStream(socket_t *sockEch, buffer_t buff)
{
    CHECK(recv(sockEch->fd, buff, MAX_BUFFER, 0), "Can't receive");
}


/**
 *	\fn			void _recevoirMessDGRAM(socket_t *sockEch, buffer_t buff, char *adrIP, int *port)
 *	\brief		Réception d'un message sur une socket de type DGRAM
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		buff : message reçu
 *	\param 		adrIP : adresse IP de l'expéditeur
 *	\param 		port : port de l'expéditeur
 *	\result		paramètre sockEch modifié
 */
void _recevoirMessDGRAM(socket_t *sockEch, buffer_t buff)
{
    struct sockaddr_in addrSrc;
    socklen_t len = sizeof(addrSrc);
    recvfrom(sockEch->fd, buff, MAX_BUFFER, 0, (struct sockaddr *)&addrSrc, &len);
}


/**
 *	\fn			void _serial(generic quoi, buffer_t buff)
 *	\brief		Sérialisation d'une requête/réponse
 *	\param 		quoi : requête/réponse à sérialiser
 *	\param 		buff : buffer de sérialisation
 *	\result		paramètre buff modifié
 */
void _serialMess(generic quoi, buffer_t buff)
{
    strcpy(buff, (char *) quoi);
}


/**
    *	\fn			void _deSerial(buffer_t buff, generic quoi)
    *	\brief		Dé-sérialisation d'une requête/réponse
    *	\param 		buff : buffer de dé-sérialisation
    *	\param 		quoi : requête/réponse à remplir
    *	\result		paramètre quoi modifié
*/
void _deSerialMess(buffer_t buff, generic quoi)
{
    strcpy((char *)quoi, buff);
}

