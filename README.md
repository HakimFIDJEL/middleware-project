# PROJET Middleware | DISCORD CLONE EN C
# FIDJEL Hakim - BOUGHRIET Younes

# - Objectifs


# Réaliser un clone discord qui aura les fonctionnalités suivante

# Tous les utilisateurs se connectent à un serveur global où chaque utilisateur peut discuter
# Des commandes sont implémentées et appelées par /{command} arg
# Les utilisateurs peuvent créer des salons de discutions privés avec /group create
# Ils peuvent inviter d'autres personnes dans ce salon avec /group invit {group_id} {user} de puis le groupe en question
# Quitter le groupe grâce à /group quit et s'y reconnecter avec /group join {group_id}
# Supprimer le groupe grâce à /group destroy {group_id}, seul le créateur du groupe (celui qui a fait /group create)

# Une navigation entre le serveur global et les groupes privés sera réalisée grâce à une bibliothèque graphique en C pour l'invite de commande
# Les utilisateurs une fois le client lancé se connectent au serveur global grâce à /connect et se déconnecte grâce à /disconnect



# - Rappels des commandes


# /home
# /group create
# /group invit {group_id} {user}
# /group kick {group_id} {user}
# /group join {group_id}
# /group quit
# /group destroy {group_id}
# /group list
# /connect
# /disconnect



# - Fonctionnement / TODO


# Mise en place d'un serveur classique repris du cours permettant la connexion de plusieurs clients à un même serveur - TODO
# Mise en place de fork des serveurs afin de supporter la multicommunication en simultannée sans fil d'attente - TODO
# Découverte de ncurses pour l'affichage graphique
# Fonction qui récupère une string et en déduit si c'est une commande ou non, si oui, appel la fonction de la commande avec les arguments, sinon, appel une fonction de display
# Affichage des messages des autres utilisateurs dans l'interface client
# Création, Invitations, et autres fonctions relatives aux groupes
# Navigations entre le serveur global et les groupes
