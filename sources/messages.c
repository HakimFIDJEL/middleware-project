/**
 *	\file		messages.c
 *	\brief		Spécification de la couche Messages
 *	\author		Hakim FIDJEL
 *	\date		25 mars 2024
 *	\version	1.0
 */

#include "../heads/messages.h"

int MESSAGE_ID = 0;
Message messages[MAX_MESSAGES];

/**
 * @brief Create if not exists one file for each channel in the ./tmp directory to store messages
 * @fn void init_messages(Channel *channels)
 * @param channels tableau de channels
 */
void init_messages(Channel *channels)
{
    char path[100];

    if (channels == NULL)
    {
        fprintf(stderr, "Erreur: Le tableau de channels est NULL.\n");
        return;
    }

    for (int i = 0; i < MAX_CHANNELS; i++)
    {

        if (channels[i].id == -1)
        {
            continue;
        }

        sprintf(path, "%s%d", tmp_path, channels[i].id);

        FILE *file = fopen(path, "a");
        if (file == NULL)
        {
            fprintf(stderr, "Erreur : Impossible de créer ou ouvrir le fichier %s\n", path);
        }
        else
        {
            fclose(file);
        }
    }
}

/**
 * @brief Store a message in the correct file
 * @fn void store_message(int channel, int user, buffer_t buff)
 * @param channel The channel where the message is sent
 * @param user The user who sent the message
 * @param buff The message to store
 */
void store_message(int channel, int user, buffer_t buff)
{
    char path[100];
    int id = MESSAGE_ID++;

    sprintf(path, "%s%d", tmp_path, channel);

    FILE *file = fopen(path, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", path);
        return;
    }

    time_t timestamp;
    time(&timestamp);

    fprintf(file, "%d#%d#%s#%ld\n", id, user, buff, timestamp);

    fclose(file);

    return;
}

/**
 * @brief Get the messages object
 * @fn Message *get_messages(int channel)
 * @param channel The channel to get the messages from
 * @return Message* 
 */
Message *get_messages(int channel)
{
    char path[100];
    sprintf(path, "%s%d", tmp_path, channel);

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", path);
        return NULL;
    }

    int i = 0;
    while (fscanf(file, "%d#%d#%s#%ld\n", &messages[i].id, &messages[i].user, messages[i].buff, &messages[i].timestamp) != EOF)
    {
        i++;
    }

    fclose(file);

    return messages;
}

/**
 * @brief Display all messages from the file
 * @fn void display_messages(Message *messages)
 * @param messages The messages to display
 */
void display_messages(Message *messages)
{
    if (messages == NULL)
    {
        fprintf(stderr, "Erreur : Le tableau de messages est NULL.\n");
        return;
    }

    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        if (messages[i].id == -1)
        {
            break;
        }

        printf("%d#%d#%s#%ld\n", messages[i].id, messages[i].user, messages[i].buff, messages[i].timestamp);
    }

    return;
}

/**
 * @brief Delete all messages from the file of the given channel and the file
 * @param channel The channel to delete the messages from
 */
void delete_messages(int channel)
{
    char path[100];
    sprintf(path, "%s%d", tmp_path, channel);

    if (remove(path) != 0)
    {
        fprintf(stderr, "Erreur : Impossible de supprimer le fichier %s\n", path);
    }

    return;
}
