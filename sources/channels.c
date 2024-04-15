/**
 *	\file		users.h
 *	\brief	    Fichier d'en-tête du module des utilisateurs
 *	\author		Hakim FIDJEL
 *	\date		3 avril 2024
 *	\version	1.0
 */
/*
*****************************************************************************************
 *	\noop		I N C L U D E S   S P E C I F I Q U E S
 */
#include "../heads/channels.h"

Channel channels[MAX_CHANNELS]; /**< tableau des channels */

/*
*****************************************************************************************
 *	\noop		P R O T O T Y P E S   DES   F O N C T I O N S
 */

/**
 *	\fn			void init_channels()
 *	\brief		initialisation des channels
 *	\note	    Cette fonction initialise le tableau de channels
 *	\result	    void
 */
void init_channels()
{
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        channels[i].id = -1;
        for (int j = 0; j < MAX_USERS; j++)
        {
            channels[i].users[j] = -1;
        }
    }
    printf("[init_channels] Tableau de channels initialisé\n");

    return;
}

/**
*	\fn			 add_channel(int id, User host, char name[50])
*	\brief		Ajout d'un channel
*	\param		int id			id du channel
*	\param		User host		hôte du channel
*	\param		char name[50]	nom du channel
*	\note	    Cette fonction ajoute un channel au tableau
*	\result	    Channel
 */
Channel add_channel(User host, char name[50])
{
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == -1)
        {
            channels[i].id = i;
            channels[i].host = host;
            strcpy(channels[i].name, name);
            printf("[add_channel] %s [%d] added\n", name, i);
            add_user_to_channel(host, &channels[i]);
            return channels[i];
        }
    }
}

/**
 * \fn		    remove_channel(Channel channel)
 * \brief		Suppression d'un channel 
 * \param		Channel channel		pointeur sur le channel à supprimer
 * \note	    Cette fonction supprime un channel du tableau
 * \result	    void
 */
void remove_channel(Channel* channel)
{
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == channel->id)
        {
            channels[i].id = -1;
            for (int j = 0; j < MAX_USERS; j++)
            {
                channels[i].users[j] = -1;
            }
            printf("[remove_channel] %s removed\n", channel->name);
        }
    }

    return;
}

/**
 * \fn         isUserInChannel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      User user      utilisateur à vérifier
 * \param      Channel channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
*/
bool is_user_allowed_in_channel(User user, Channel channel){

    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel.users[i] == user.id)
        {
            return true;
        }
    }

    return false;
}

bool is_user_in_channel(User user, Channel channel){

    if(user.currentChannel == channel.id)
    {
        return true;
    }
    return false;
}

/**
 * \fn         displayChannels()
 * \brief      Affiche les channels
 * \note       Cette fonction affiche les channels
 * \result     void
*/
void display_channels(User user){

    printf("*******************************************\n");
    printf("[display_channels] Name | ID | Host\n");
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if(channels[i].id == -1)
        {
            continue;
        }
        if (channels[i].id != -1 && is_user_allowed_in_channel(user, channels[i]))
        {
            printf("%s | %d | %s \n", channels[i].name, channels[i].id, channels[i].host.name );

        }

    }
    printf("*******************************************\n");



   

}

void display_users_in_channel(Channel channel)
{
    if(channel.id == -1)
    {
        return;
    }
    printf("*******************************************\n");
    printf("[display_users_in_channel] Users in %s\n", channel.name);
    for (int i = 0; i < MAX_USERS; i++)
    {
        // if (channel.users[i] != -1)
        // {
            printf("%d | ", channel.users[i]);
        // }-
    }
    printf("\n*******************************************\n");
}




Channel *get_channel_by_id(int id) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (channels[i].id == id) {
            return &channels[i]; // Retourner l'adresse du channel trouvé
        }
    }
    return NULL; // Retourner NULL si aucun channel correspondant n'est trouvé
}



void add_user_to_channel(User user, Channel *channel)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel->users[i] == -1)
        {
            channel->users[i] = user.id;
            printf("[add_user_to_channel] USER_ID = %d | CHANNEL.USERS[i] = %d\n", user.id, channel->users[i]);
            printf("[add_user_to_channel] %s added to channel %s\n", user.name, channel->name);

            return;
        }
    }

    printf("[add_user_to_channel] %s not added to channel %s\n", user.name, channel->name);

    return;
}

void remove_user_from_channel(User user, Channel *channel)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel->users[i] == user.id)
        {
            channel->users[i] = -1;
            printf("[remove_user_from_channel] %s removed from channel %s\n", user.name, channel->name);
            return;
        }
    }

    printf("[remove_user_from_channel] %s not removed from channel %s\n", user.name, channel->name);

    return;
}
