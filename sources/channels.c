/**
 *	\file		channels.c
 *	\brief	    Spécification de la couche Channels
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
 *	\fn			add_channel(int id, User host, char name[50])
 *	\brief		Ajout d'un channel
 *	\param		host		hôte du channel
 *	\param		name[50]	nom du channel
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
 * \param		channel		pointeur sur le channel à supprimer
 * \note	    Cette fonction supprime un channel du tableau
 * \result	    void
 */
void remove_channel(Channel *channel)
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

            return;
        }
    }

    return;
}

/**
 * \fn         is_user_allowed_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      user      utilisateur à vérifier
 * \param      channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
 */
bool is_user_allowed_in_channel(User user, Channel channel)
{

    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel.users[i] == user.id)
        {
            return true;
        }
    }

    return false;
}

/**
 * \fn         is_user_in_channel(User user, Channel channel)
 * \brief      Vérifie si un utilisateur est dans un channel
 * \param      user      utilisateur à vérifier
 * \param      channel    channel à vérifier
 * \note       Cette fonction vérifie si un utilisateur est dans un channel
 * \result     bool
 */
bool is_user_in_channel(User user, Channel channel)
{

    if (user.currentChannel == channel.id)
    {
        return true;
    }
    return false;
}

/**
 * @brief display channels
 * @fn void display_channels(User user, buffer_t buff)
 * @param user The user to display the channels to
 * @param buff The buffer to display the channels to
 */
void display_channels(User user, buffer_t buff)
{
    char id[10];
    buff[0] = '\0';

    printf("[display_channels] %s", user.name);

    strcat(buff, "\n*******************************************\n");
    strcat(buff, "[display_channels] Name | ID | Host\n");

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == -1)
        {
            continue;
        }
        if (channels[i].id != -1 && is_user_allowed_in_channel(user, channels[i]))
        {

            strcat(buff, " - \t ");
            strcat(buff, channels[i].name);
            strcat(buff, " | ");

            sprintf(id, "%d", channels[i].id);
            strcat(buff, id);
            strcat(buff, " | ");
            strcat(buff, channels[i].host.name);
            strcat(buff, "\n");
        }
    }
    strcat(buff, "\n*******************************************\n");

    return;
}

/**
 * @brief display users in channel
 * @fn void display_users_in_channel(Channel channel, buffer_t buff)
 * @param channel The channel to display the users from
 * @param buff The buffer to display the users to
 * @return void
 */
void display_users_in_channel(Channel channel, buffer_t buff)
{
    buff[0] = '\0';
    char id[10];

    if (channel.id == -1)
    {
        return;
    }

    printf("[display_users_in_channel] %s\n", channel.name);

    strcat(buff, "\n*******************************************\n");
    strcat(buff, "[display_users_in_channel] Users in ");
    strcat(buff, channel.name);
    strcat(buff, "\n");
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel.users[i] != -1)
        {
            sprintf(id, "[ID : %d]", channel.users[i]);
            strcat(buff, id);
            if (i < MAX_USERS - 1)
            {
                strcat(buff, " | ");
            }
        }
    }
    strcat(buff, "\n*******************************************\n");

    return;
}

/**
 * @brief Get the channel by id
 * @fn Channel *get_channel_by_id(int id)
 * @param id The id of the channel
 * @return Channel* 
 */
Channel *get_channel_by_id(int id)
{
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == id)
        {
            return &channels[i]; // Retourner l'adresse du channel trouvé
        }
    }
    return NULL; // Retourner NULL si aucun channel correspondant n'est trouvé
}

/**
 * @brief add user to channel
 * @fn void add_user_to_channel(User user, Channel *channel)
 * @param user The user to add
 * @param channel The channel to add the user to
 * @return void
 */
void add_user_to_channel(User user, Channel *channel)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (channel->users[i] == -1)
        {
            channel->users[i] = user.id;
            printf("[add_user_to_channel] %s added to channel %s\n", user.name, channel->name);

            return;
        }
    }

    printf("[add_user_to_channel] %s not added to channel %s\n", user.name, channel->name);

    return;
}

/**
 * @brief remove user from channel
 * @fn void remove_user_from_channel(User user, Channel *channel)
 * @param user The user to remove
 * @param channel The channel to remove the user from
 * @return void
 */
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

/**
 * @brief get channels
 * @fn Channel *get_channels()
 * @return Channel* 
 */
Channel *get_channels()
{
    return channels;
}