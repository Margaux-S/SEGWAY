# Temps réel

Le programme de temps réel est exécuté sur la carte Raspberry Pi 3. Ce programme gère le monitoring du système, ainsi que l'asservissement en angle du Segway.
Nous avons distribué les fonctions en 6 threads :

ps:
*false* =0;  *true* = 1
### Communication
------------------------------------------------------


Le thread **Communication** se charge de récupérer périodiquement, à une fréquence de 50Hz, les informations reçues à l’aide de la liaison série du STM32. Il déchiffre les trames et mettre à jour les variables partagées utilisées par les autres threads.

Il doit d’abord vérifier que la communication est bien établie. Si ce n’est pas le cas, il met à *false* la variable *état_com*. Le thread se met ensuite en attente d’une trame sur la liaison série.

A la réception de celle-ci, les données sont extraites et les variables partagées sont mises à jour en fonction, et la variable *état_com* est mise à l’état *true* et la variable *etat_reception* est mise à l’état *true*. S'il attend trop de temps, il va sortir de boucle et afficher Perte de communication. Il met à jours de *état_com* à *false* et la variable *etat_reception* est mise à l’état *false*.

Le thread est également chargé d’envoyer une trame d’arrêt du système si la variable *état_com* est à l’état *false*.
Ce thread fonctionne à une fréquence de 50 Hz.

### Asservissement
------------------------------------------------------

Le thread **Asservissement** effectue les calculs nécessaires au contrôle des déplacements du Segway.
Si la variable *etat_com* est égale à *true*, il verifie si le trame est bien reçu via la variable de *etat_reception* .

Si tous est bon, il récupère les informations d’angle au niveau de la variable partagée *etat_angle*, et après son calcul, écrit les résultats dans la variable *consigne_courant*; puis l'envoie dans la file de message et met la variable *etat_reception*  à l’état *false*.
Ce thread fonctionne à une fréquence de 50 Hz.


### Surveillance Batterie
------------------------------------------------------

Le thread **Surveillance Batterie** se charge de vérifier le bon fonctionnement des moteurs. Il consulte périodiquement, à une fréquence de 1 Hz, les informations contenues dans la variable partagée *batterie*, tant que la variable *etat_com* est à l’état *true*.
Enfin si le niveau de batterie atteint un niveau critique, il déclenche l’arrêt d’urgence par semaphore. Il remets aussi *arrêt* est mise à l’état *false* si les conditions de batterie et presence user sont remplis

### Affichage
------------------------------------------------------

Le thread **Affichage** se charge de communiquer à l’utilisateur toutes les informations importantes récoltées par le STM32. Pour cela il consulte certaines variables partagées et affiche leur état. Les valeurs sont rafraîchies à 10 Hz pour une bonne lisibilité.

### Présence User
------------------------------------------------------

Le thread **Présence User** vérifie de manière périodique à 10 Hz la présence de l’utilisateur en consultant la variable partagée *présence_user*. Si cette variable est à l’état *false*, un compteur est incrémenté. Lorsque le compteur est égal à 5 (correspond à 500 ms d’absence d’appui sur le bouton), la variable *arrêt* est mise à l’état *true*, afin que l’arrêt d’urgence soit déclenché. De la même manière, le thread consulte la variable *etat_com*, et grâce à un autre compteur, il vérifie la durée du problème de communication et demande l’arrêt du système si ce compteur est égal à 2 (200 ms de perte de communication).

### Arrêt Urgence
------------------------------------------------------

Le thread **Arrêt Urgence** est déclenché par le sémaphore arret (qui peut être déclenché par le thread Surveillance Batterie ou Présence User). Nous avons choisi un fonctionnement apériodique car cette tâche a seulement besoin d'être exécutée lorsqu'il est nécessaire d'arrêter le système, un fonctionnement périodique aurait fait consommer des ressources inutilement. Lorsque la tâche est déclenchée, la variable partagée arrêt est mise à true, et un message de type arrêt (de label "a"), avec la valeur égale à "1" est envoyée à la file de message.

### Envoyer
------------------------------------------------------

Le thread **Envoyer** envoie le trame au STM tous les 100 Hz en lisant le file de message
