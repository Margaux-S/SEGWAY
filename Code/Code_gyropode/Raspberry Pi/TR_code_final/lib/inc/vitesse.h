/**
 * \file vitesse.h
 * \brief Déclaration de la classe Vitesse.
 * \author INSA Toulouse
 * \version 1.0
 * \date 25 Juillet 2017
 */

#ifndef VITESSE_H
#define VITESSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "constantes.h"

class Vitesse{
	private :
		float _vitesse;
	public :
		Vitesse();
		void set_vitesse(float vitesse);
		float vitesse();
		~Vitesse(void);
};
#ifdef __cplusplus
}
#endif
#endif
