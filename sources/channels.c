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
void init_channels(){
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        channels[i].id = -1;
    }
    printf("Tableau de channels initialisé\n");

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
Channel add_channel(int id, User host, char name[50]){

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == -1)
        {
            channels[i].id = id;
            channels[i].host = host;
            strcpy(channels[i].name, name);
            return channels[i];
        }
    }
    return channels[0];

}

/**
 * \fn		    remove_channel(Channel channel)
 * \brief		Suppression d'un channel 
 * \param		Channel channel		pointeur sur le channel à supprimer
 * \note	    Cette fonction supprime un channel du tableau
 * \result	    void
 */
void remove_channel(Channel channel){

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == channel.id)
        {
            channels[i].id = -1;
            printf("Channel \"%s\" [%d] removed\n", channel.name, channel.id);
        }
    }

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
        if (channel.users[i].id == user.id)
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
void display_channels(){

    printf("\n****************************************************\n");
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id != -1)
        {
            printf("- Channel %d : %s\n", channels[i].id, channels[i].name);
        }
    }
    printf("****************************************************\n");

}


Channel get_channel_by_id(int id){

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].id == id)
        {
            return channels[i];
        }
    }
    return channels[0];
}