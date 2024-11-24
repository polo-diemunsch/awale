# Bienvenue sur Awale

# Exécution
Pour exécuter le serveur, exécutez le fichier server dans le dossier bin, par exemple en utilisant la commande `./bin/server`.
Pour exécuter un client, exécutez le fichier client dans le dossier bin, par exemple en utilisant la commande `./bin/client adresse_du_serveur nom_du_client`.
Le jeu est fourni avec les exécutables prêts.  
Vous pouvez les recompiler en utilisant la commande `make client server` dans un terminal, depuis le dossier awale.

# Jeu

                                                     AWALÉ

                                                    GENERAL
                    online			   print the list of online players
                    send [player|all] message    send a message to a player / to everyone
                    challenge player             challenge a player to a duel / accept
                                                the challenge from a player
                    decline_challenge player     decline a challenge from a player /
                                                cancel a challenge sent to a player
                    historic (n)                 list your (n) past games
                    replay game                  watch the replay of a game

                                                    IN GAME
                    play slot                    choose which slot to play

                                                    REPLAY
                    next                         display next move
                                      
## GENERAL
### Online
Utilisez la commande `online` pour afficher les pseudos de tous les clients connectés au serveur. 
### Chat
Il est possible d'envoyer un message visible par tous les clients ou bien de l'envoyer à un client en particulier.
### Challenge
Pour inviter un client à jouer une partie d'Awale, utilisez la commande `challenge nom_du_joueur`. Le joueur recevra une invitation qu'il pourra accepter via `challenge votre_nom`.
## Historique
Pour afficher la liste des dernières parties jouées, utilisez `historic`. Pour afficher tout le déroulé d'une partie, utilisez `replay game nom_du_jeu`.
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

## REPLAY
Lorsque vous visionnez une partie en replay via l'historique, utilisez `next` pour passer à l'état de plateau suivant. 


# Quitter
Pour quitter, entrez exit ou arrêtez simplement le processus depuis votre terminal, en utilisant Ctrl+C par exemple. 
Si vous gérez les clients et le serveur, quittez les clients en premier. 
