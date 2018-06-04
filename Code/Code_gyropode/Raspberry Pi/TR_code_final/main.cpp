/* 
 * File:   main.c
 * Author: INSA Toulouse
 *
 * Created on 11-07
 */
/*#!/bin/bash
gnome-terminal -e ./../GUI/GUI*/

#include "includes.h"
#include <iostream>

void initStruct(void);
void startTasks(void);
void deleteTasks(void);

int main(void){

	/* disable memory swap */
	mlockall(MCL_CURRENT | MCL_FUTURE);	
	init_recording();
	initStruct();
	startTasks();
	pause();
        printf("pause\n");
	deleteTasks();
	stop_recording();
	return 0;
}

void initStruct(void) {
	int err;

	/* Creation des mutex */

	if(err = rt_mutex_create(&var_mutex_etat_com, "mutex_etat_com")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_vitesse, "mutex_vitesse")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_etat_angle, "mutex_etat_angle")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_consigne_couple, "mutex_consigne_couple")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_status, "mutex_status")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_batterie, "mutex_batterie")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_etat_reception, "mutex_reception")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_presence_user, "mutex_presence_utilisateur")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_mutex_create(&var_mutex_arret, "mutex_arret")){
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

        
	rt_printf("Fin init mutex\n");

	/* Creation des semaphores */

	if(err = rt_sem_create(&var_sem_arret, "var_sem_arret",0,TM_INFINITE)){
		rt_printf("Error semaphore create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if(err = rt_sem_create(&var_sem_envoyer, "var_sem_envoyer",0,TM_INFINITE)){
		rt_printf("Error semaphore create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	rt_printf("Fin init semaphores\n");

	/* Creation des taches */

	if (err = rt_task_create(&th_Communication, "Communication", 0, PRIORITY_TH_COMMUNICATION, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Asservissement, "Asservissement", 0, PRIORITY_TH_ASSERVISSEMENT, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Affichage, "Affichage", 0, PRIORITY_TH_AFFICHAGE, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Surveillance_Batterie, "Surveillance_Batterie", 0, PRIORITY_TH_BATTERIE, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Presence_User, "Presence_User", 0, PRIORITY_TH_PRESENCEUSER, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Envoyer, "Envoyer", 0, PRIORITY_TH_ENVOYER, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_create(&th_Arret_Urgence, "Arret_Urgence", 0, PRIORITY_TH_ARRET_URGENCE, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
        
        if (err = rt_task_create(&th_Communication_Android, "Communication_Android", 0, PRIORITY_TH_COMMUNICATION_ANDROID, 0)){
		rt_printf("Error task create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
        

    rt_printf("Fin init tache\n");

	/** Création de la file de message */

	int err1 = 0;
			err1 = rt_queue_create(&queue_Msg2STM, "Queue_stm", sizeof(message_stm)*1000, Q_UNLIMITED, Q_FIFO);
			if (err1<0){
				switch(err){
                                   
					case (-EINVAL):
						rt_printf("rt_queue_create error: invalid mode or poolsize is 0\n");
						break;
					case (-ENOMEM):
						rt_printf("rt_queue_create error: system fails to get memory \n");
						break;
					case (-EEXIST):
						rt_printf("rt_queue_create error: name conflict \n");
						break;
					case (-EPERM):
						rt_printf("rt_queue_create error: called by asynchronous context \n");
						break;
				}
			}
			else rt_printf("Queue créée avec succès\n");
}

void startTasks() {
	int err;

	if (err = rt_task_start(&th_Communication, &Communication, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_start(&th_Asservissement, &Asservissement, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_start(&th_Affichage, &Affichage, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_start(&th_Surveillance_Batterie, &Surveillance_Batterie, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_start(&th_Presence_User, &Presence_User, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
	
	if (err = rt_task_start(&th_Envoyer, &Envoyer, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	if (err = rt_task_start(&th_Arret_Urgence, &Arret_Urgence, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
        
        if (err = rt_task_start(&th_Communication_Android, &Communication_Android, NULL)){
		rt_printf("Error task start: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

}
void deleteTasks() {

	rt_task_delete(&th_Communication);

	rt_task_delete(&th_Asservissement);

	rt_task_delete(&th_Affichage);

	rt_task_delete(&th_Surveillance_Batterie);

	rt_task_delete(&th_Presence_User);

	rt_task_delete(&th_Envoyer);

	rt_task_delete(&th_Arret_Urgence);
                                 
        rt_task_delete(&th_Communication_Android);

}


