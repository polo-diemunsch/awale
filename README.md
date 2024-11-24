# Bienvenue sur Awale

# Exécution
Pour exécuter le serveur, exécutez le fichier ./bin/server depuis le dossier awale en exécutant la commande `./bin/server`.
Pour exécuter un client, exécutez le fichier ./bin/client depuis le dossier awale en exécutant la commande `./bin/client adresse_du_serveur pseudo mot_de_passe`.
Le jeu est fourni avec les exécutables prêts. 
Vous pouvez les recompiler en utilisant la commande `make client server` dans un terminal, depuis le dossier awale.
Les couples pseudos/mdp disponibles sont : 
polo mdp1
nor mdp2
D'autres peuvent être créés, tout simplement en ajoutant un fichier texte dans ./data depuis le dossier awale, ayant pour titre le pseudo et contenu le mot de passe.
La taille maximale pour un nom et un mot de passe est 128 caractères.

# Jeu

                                 AWALÉ

                  online                       show online players
                  send [player|all] message    send a message to a player / to everyone
                  challenge player             challenge a player to a duel / accept
                                               the challenge from a player
                  decline_challenge player     decline a challenge from a player /
                                               cancel a challenge sent to a player
                  play slot                    choose which slot to play
                  forfeit                      forfeit the game
                  list_games                   list games currently playing
                  spectate [player|game]       spectate a player or a game
                  history (n)                 list your past games
                  home                         stop spectating / replaying a game

                                      
## GENERAL
### Online
Utilisez la commande `online` pour afficher les pseudos de tous les clients connectés au serveur. 
### Chat
Il est possible d'envoyer un message visible par tous les clients ou bien de l'envoyer à un client en particulier.
### Challenge
Pour inviter un client à jouer une partie d'Awale, utilisez la commande `challenge nom_du_joueur`. Le joueur recevra une invitation qu'il pourra accepter via `challenge votre_nom` ou refuser via `decline_challenge votre_nom`.

## Historique
Pour afficher la liste des dernières parties jouées, utilisez `history`. 

### Replay
Le replay n'a pas encore été mis en place.
Voici la démarche à suivre si vous souhaitez le mettre en place vous-même :
- récupérer le nom du jeu auprès du joueur
- lire l'historique du game dans son fichier txt (première ligne : joueur 1, deuxième ligne : joueur 2, troisième ligne : premier joueur, quatrième ligne : direction, puis une ligne par round avec le slot joué). 
- utiliser les fonctions de awale.c (create_game, execute_round) en les adaptant (retirer les tests de move valides et les enregistrements dans les fichiers txt) pour rejouer la partie sur le poste client.
- ou bien rejouer la partie sur le serveur et utiliser les fonctions de communication.c (send_game_init_to_client et send_game_state_to_client)


## IN GAME
Exemple de display de début de partie :

                                                     AWALÉ


                                    Turn 0 - Waiting for opponent to play


                                                Y (Opponent)

                                        ╭────┬────┬────┬────┬────┬────╮
                                    0   │  4 │  4 │  4 │  4 │  4 │  4 │  ◀─╮
                                        ├────┼────┼────┼────┼────┼────┤    │
                                    0   │  4 │  4 │  4 │  4 │  4 │  4 │   ─╯
                                        ╰────┴────┴────┴────┴────┴────╯
                                        a    b    c    d    e    f

                                                    R (You)

Les informations que vous voyez sont : 
- à quel round la partie en est
- si vous devez jouer ou attendre que votre opposant.e joue
- le plateau, avec le nombre de grains contenus dans chaque case
- en face, le nom de votre opposant.e et son score, à gauche du plateau
- votre nom et votre score, à gauche du plateau
- le sens de distribution des graines
- les trous sont repérés par des lettres de a à f

A votre tour, vous pouvez distribuer les graines du trou de votre choix en utilisant la commande `play lettre_de_la_case`. 
Vous pouvez abandonner avec `forfeit`.

## SPECTATE
Vous pouvez voir la liste des parties en cours en utilisant `list_games`.
Vous pouvez observer une autre partie en cours grâce à `spectate [player|game]`. Revenez à l'écran d'accueil en utilisant `home`.

# Quitter
Pour quitter, entrez exit ou arrêtez simplement le processus depuis votre terminal, en utilisant Ctrl+C par exemple. 
Si vous gérez les clients et le serveur, quittez les clients en premier. 

