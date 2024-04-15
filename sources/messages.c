#include "../heads/messages.h"


int MESSAGE_ID = 0;
Message messages[MAX_MESSAGES];


// Create if not exists one file for each channel in the ./tmp directory to store messages
void init_messages(Channel *channels)
{
    char path[100];


    if (channels == NULL) {
        fprintf(stderr, "Erreur: Le tableau de channels est NULL.\n");
        return;
    }

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        

        if(channels[i].id == -1)
        {
            continue;
        }

        sprintf(path, "%s%d", tmp_path, channels[i].id);
        

        FILE *file = fopen(path, "a");
        if (file == NULL) {
            fprintf(stderr, "Erreur : Impossible de créer ou ouvrir le fichier %s\n", path);
        } else {
            fclose(file);
        }


    }
}

// Store a message in the correct file
void store_message(int channel, int user, buffer_t buff)
{
    char path[100];
    int id = MESSAGE_ID++;

   
    sprintf(path, "%s%d", tmp_path, channel);

    FILE *file = fopen(path, "a");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", path);
        return;
    }

    time_t timestamp;
    time(&timestamp);

    fprintf(file, "%d#%d#%s#%ld\n", id, user, buff, timestamp);

    fclose(file);

    return;
}

// Retrieve all messages from the file of the given channel
Message* get_messages(int channel)
{
    char path[100];
    sprintf(path, "%s%d", tmp_path, channel);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", path);
        return NULL;
    }

    int i = 0;
    while (fscanf(file, "%d#%d#%s#%ld\n", &messages[i].id, &messages[i].user, messages[i].buff, &messages[i].timestamp) != EOF) {
        i++;
    }

    fclose(file);

    return messages;
}

// Display all messages from the file
void display_messages(Message *messages)
{
    if (messages == NULL) {
        fprintf(stderr, "Erreur : Le tableau de messages est NULL.\n");
        return;
    }

    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (messages[i].id == -1) {
            break;
        }

        printf("%d#%d#%s#%ld\n", messages[i].id, messages[i].user, messages[i].buff, messages[i].timestamp);
    }

    return;
}

// Delete all messages from the file of the given channel and the file
void delete_messages(int channel)
{
    char path[100];
    sprintf(path, "%s%d", tmp_path, channel);

    if (remove(path) != 0) {
        fprintf(stderr, "Erreur : Impossible de supprimer le fichier %s\n", path);
    }

    return;
}
