# Matlab
Ce dossier Matlab contient les codes pour simuler et trouver les lois d'automatique pour la commande du Segway. 

La loi d'asservissement du système complet est donné par le professeur INSA Toulouse, M. Ion Hazyuk. 
Il s'agit la commande par correcteur retour d'état.

--------------------------------------------------------------------------------------------------------------

### Système Complet

Dans notre système, la consigne pour theta (angle entre l'axe vertical du sol et le guidon) est égale à zéro car nous voulons stabiliser le système et garder le Segway en position verticale.

Le système est donc commandé par Beta (angle entre l'utilisateur et le guidon), qui est considéré comme une pertubation du système.
L'asservissement du système se fait dans la carte Raspberry Pi et il envoie tous les 50 Hz à la carte STM32 pour la commande en couple.

                     | Beta
                     |
    Couple   ________|___________________       theta (angle)
       -----|  Système mécanique         |----------------------------------
       |    |  du segway                 |                                  |
       |    |                            |    d_theta(vitesse angulaire)    |
       |    |____________________________|-------------------------------   |
       |                                                                 |  |
       |           _____      ____                                       |  |
       |----------|  +  |----| k2 |--------------------------------------|  |
                  |     |    |____|                                         |
                  |  +  |     ____                                          |
                  |_____|----| k1 |-----------------------------------------
                             |____|
                             
----------------------------------------------------------------------------------------------------------------------------------

### Système boucle de courant

Dans notre système, la consigne de couple est reçue tous les 50 Hz par le STM32, elle est d'abord convertie en consigne de courant correspondant au couple du retour d'état.

Le moteur est commandé en tension par PWM pour atteindre le couple souhaité par le système et on utilise un correcteur PI pour la régulation de courant (voir dossier STM32).

                             
         (en A)         err(en A)    ________    Commande (en V)     ______
     Consigne_I-->+( )--------------| Cor_PI |----------->----------|Moteur|--->------- I
                   -^               |________|                      |______|    |
                    |                                                           |
                    |-----------------------------------------------------------|
							 		
