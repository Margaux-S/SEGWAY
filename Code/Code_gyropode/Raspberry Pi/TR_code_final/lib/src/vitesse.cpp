/**
 * \file vitesse.cpp
 * \brief Source de la classe Vitesse
 * \author INSA Toulouse
 * \version 1.0
 * \date 25 Juillet 2017
 *
 * Implémentation de la classe Vitesse
 *
 */

#include "../inc/vitesse.h"

/**
     * \fn Constructeur de classe
     * \brief Constructeur de l'objet Vitesse.
     */
Vitesse::Vitesse(){
	_vitesse = VITESSE_LIN_INITIAL  ;
}

/**
     * \fn void set_vitesse(float vitesse)
     * \brief Mise à jour de la vitesse de l'objet Vitesse.
     * \param vitesse Nouvelle valeur de la vitesse lineaire de segway
     */
void Vitesse :: set_vitesse(float vitesse){
	this->_vitesse = vitesse;
}

/**
     * \fn float vitesse()
     * \brief Accesseur de la valeur _vitesse  de l'objet Vitesse.
     * \return _vitesse
     */
float Vitesse :: vitesse(){
	return _vitesse;
}

/**
     * \fn Destructeur de classe
     * \brief Destructeur de l'objet Vitesse.
     */
Vitesse :: ~Vitesse(void){}

