/* 
 * File:   global.h
 * Author: INSA Toulouse
 *
 * Created on 20 May 2017
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include "includes.h"
#include "lib/lib.h"

/* Structures ******************************/
typedef struct{
	char label;
	float data;
}info_float;

typedef struct{
	char label;
	int data;
}info_int;

/* Defines *********************************/
#define STOP 99
#define k1 -25.8598f
#define k2 -6.5593f
#define PI  3.142f
#define TEN_SECONDS    (10*1e9)
#define SECENTOP       1200000000
#define MSG_LENGTH 50
#define MSG_QUEUE_SIZE 20
#define DEB rt_f("%d\n",__LINE__);
 
/* Priorités *******************************/
#define PRIORITY_TH_COMMUNICATION 80
#define PRIORITY_TH_ASSERVISSEMENT 81
#define PRIORITY_TH_AFFICHAGE 70
#define PRIORITY_TH_BATTERIE 73
#define PRIORITY_TH_PRESENCEUSER 75
#define PRIORITY_TH_ENVOYER 90
#define PRIORITY_TH_ARRET_URGENCE 95
#define PRIORITY_TH_COMMUNICATION_ANDROID 82

/* Variables partagées *********************/
extern int arret;
extern int status;
extern int etat_com;
extern int etat_reception;
extern int presence_user;
extern int c;
extern int android;

typedef struct
{
	char label;
	float fval;
	int ival;
} message_stm;

extern Angles etat_angle;
extern Battery batterie;
extern Consigne consigne_couple;
extern Vitesse vitesse_lin;

/* MUTEX ***********************************/
extern RT_MUTEX var_mutex_vitesse; 
extern RT_MUTEX var_mutex_etat_com; 
extern RT_MUTEX var_mutex_etat_angle;
extern RT_MUTEX var_mutex_consigne_couple;
extern RT_MUTEX var_mutex_status;
extern RT_MUTEX var_mutex_batterie;
extern RT_MUTEX var_mutex_presence_user ;
extern RT_MUTEX var_mutex_etat_reception;
extern RT_MUTEX var_mutex_arret ;

/* SEMAPHORES ******************************/
extern RT_SEM var_sem_arret;
extern RT_SEM var_sem_envoyer;

/* Files de messages ************************/
extern RT_QUEUE queue_Msg2STM;

/* Tâches **********************************/
extern RT_TASK th_Communication;
extern RT_TASK th_Asservissement;
extern RT_TASK th_Affichage; 
extern RT_TASK th_Surveillance_Batterie; 
extern RT_TASK th_Envoyer; 
extern RT_TASK th_Presence_User; 
extern RT_TASK th_Arret_Urgence; 
extern RT_TASK th_Communication_Android;

/* Fonctions *******************************/
info_float info_float_new(char lab, float dat);
info_int info_int_new(char lab, int dat);


#endif	/* GLOBAL_H */
