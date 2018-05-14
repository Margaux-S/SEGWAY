/*
 * File:   fonctions.c
 * Author: INSA Toulouse
 *
 * Created on 20 May 2017
 */

#include "fonctions.h"

void Asservissement(void *arg) /* OK */
{
	float angle, vit_angulaire, c;
	int com, android;
	int uart0_filestream;
        int noerror = 0;

	rt_printf("Thread Asservissement: Debut de l'exécution de periodique à 50 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 20000000);

	log_task_entered();

	while (1) {
	    //rt_printf("Thread Asservissement \n");
		rt_task_wait_period(NULL);

		rt_mutex_acquire(&var_mutex_etat_com, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_com);
		com = etat_com;
		rt_mutex_release(&var_mutex_etat_com);
		log_mutex_released(&var_mutex_etat_com);
                
                rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
                log_mutex_acquired(&var_mutex_etat_android);

                android = etat_android;

                rt_mutex_release(&var_mutex_arret);
                log_mutex_released(&var_mutex_arret);

		if (com){

			rt_mutex_acquire(&var_mutex_etat_angle, TM_INFINITE);
			log_mutex_acquired(&var_mutex_etat_angle);

			angle = etat_angle.angle();
			vit_angulaire = etat_angle.vitesse_ang();

			rt_mutex_release(&var_mutex_etat_angle);
			log_mutex_released(&var_mutex_etat_angle);

			rt_mutex_acquire(&var_mutex_etat_reception, TM_INFINITE);
			log_mutex_acquired(&var_mutex_etat_reception);

			com = etat_reception;

			rt_mutex_release(&var_mutex_etat_reception);
			log_mutex_released(&var_mutex_etat_reception);

			if(com) {
				c = (k1 * angle + k2 * vit_angulaire);

				rt_mutex_acquire(&var_mutex_consigne_couple, TM_INFINITE);
				log_mutex_acquired(&var_mutex_consigne_couple);
                                
                                if (not(android)){
                                    consigne_couple.set_consigne(c);
                                }    
				rt_mutex_release(&var_mutex_consigne_couple);
				log_mutex_released(&var_mutex_consigne_couple);

				rt_mutex_acquire(&var_mutex_etat_reception, TM_INFINITE);
				log_mutex_acquired(&var_mutex_etat_reception);

		  		etat_reception = 0 ;

				rt_mutex_release(&var_mutex_etat_reception);
				log_mutex_released(&var_mutex_etat_reception);

				int err=0;
				message_stm m;
				m.label = 'c';
                                
                                if (noerror){
                                    noerror = 0;
                                    m.fval = c+0.01;
                                } else {
                                    noerror = 1;
                                    m.fval = c;
				}
                                err = rt_queue_write(&queue_Msg2STM,&m,sizeof(message_stm),Q_NORMAL);

				rt_sem_v(&var_sem_envoyer);
			}
		}
	}
}

 void Presence_User(void *arg)
{
	int com;
	int pres = 0;

	rt_printf("Thread presence_user : Debut de l'éxecution de periodique à 10 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 100000000);

	log_task_entered();

	while (1) {
	    //rt_printf("Thread Presence_user \n");
		rt_task_wait_period(NULL);

		rt_mutex_acquire(&var_mutex_etat_com, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_com);

		com = etat_com;

		rt_mutex_release(&var_mutex_etat_com);
		log_mutex_released(&var_mutex_etat_com);

		if (com){

			rt_mutex_acquire(&var_mutex_presence_user, TM_INFINITE);
			log_mutex_acquired(&var_mutex_presence_user);

			pres = presence_user;

			rt_mutex_release(&var_mutex_presence_user);
			log_mutex_released(&var_mutex_presence_user);

			if(pres == 0){
				log_sem_signaled(&var_sem_arret);
				rt_sem_v(&var_sem_arret);
			}
		}
	}
}

/* Cette tâche vérifie périodiquement le niveau de batterie du gyropode
 *  déclenche l'arrêt d'urgence si la batterie est trop faible (<15%) */
 void Surveillance_Batterie(void *arg) /* OK */
{
	int com, arr, pres;
	int bat_lvl;

	rt_printf("Thread Surveillance_Batterie : Debut de l'éxecution de periodique à 1 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 1000000000);

	log_task_entered();

	while (1) {
    	//rt_printf("Thread Batterie \n");
		rt_task_wait_period(NULL);

		rt_mutex_acquire(&var_mutex_etat_com, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_com);

		com = etat_com;

		rt_mutex_release(&var_mutex_etat_com);
		log_mutex_released(&var_mutex_etat_com);

		rt_mutex_acquire(&var_mutex_presence_user, TM_INFINITE);
		log_mutex_acquired(&var_mutex_presence_user);

		pres = presence_user;

		rt_mutex_release(&var_mutex_presence_user);
		log_mutex_released(&var_mutex_presence_user);

		if (com){
			rt_mutex_acquire(&var_mutex_batterie, TM_INFINITE);
			log_mutex_acquired(&var_mutex_batterie);

			bat_lvl = batterie.level();

			rt_mutex_release(&var_mutex_batterie);
			log_mutex_released(&var_mutex_batterie);

			if(bat_lvl < 15){
				log_sem_signaled(&var_sem_arret);
				rt_sem_v(&var_sem_arret);
			} else if (pres=1) {
				rt_mutex_acquire(&var_mutex_arret, TM_INFINITE);
				log_mutex_acquired(&var_mutex_arret);
				arret = 0;
				rt_mutex_release(&var_mutex_arret);
				log_mutex_released(&var_mutex_arret);
			}
		}
	}
}


void Communication(void *arg) 
{
	int uart0_filestream = -1;
	int i;
	int com = 0;
	int timer1 = 0;
	int timer2 = 0;
	unsigned char message_buffer[256];
	int message_length = 0;
	float angular_position = 0;
	float angular_speed = 0;

	unsigned char tx_buffer[20];
	unsigned char *p_tx_buffer;
	message_serial *m;

	//rt_printf("Thread Asservissement: Debut de l'exécution de periodique à 50 Hz\n");
	//rt_task_set_periodic(NULL, TM_NOW, 20000000);

	rt_printf("Thread Asservissement: Debut de l'exécution de periodique à 100 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 10000000);

	log_task_entered();

	while (1) {
    		//rt_printf("Thread Communication \n");
			rt_task_wait_period(NULL);

			uart0_filestream = init_serial();

        	if(uart0_filestream == -1){

			rt_printf("Can't Use the UART\n");

			log_sem_signaled(&var_sem_arret);
		   	rt_sem_v(&var_sem_arret);

			rt_mutex_acquire(&var_mutex_etat_reception, TM_INFINITE);
			log_mutex_acquired(&var_mutex_etat_reception);

		  	etat_reception = 0;

			rt_mutex_release(&var_mutex_etat_reception);
			log_mutex_released(&var_mutex_etat_reception);
		}
		else{
            com = 1;
           	m = (message_serial*)malloc(5*sizeof(message_serial));;
           	m = read_from_serial();
			printf_trame(m);

			rt_mutex_acquire(&var_mutex_etat_reception, TM_INFINITE);
			log_mutex_acquired(&var_mutex_etat_reception);

	  		etat_reception = 1;

			rt_mutex_release(&var_mutex_etat_reception);
			log_mutex_released(&var_mutex_etat_reception);
        	}

		rt_mutex_acquire(&var_mutex_etat_com, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_com);

	  	etat_com = com;

		rt_mutex_release(&var_mutex_etat_com);
		log_mutex_released(&var_mutex_etat_com);
    	}
}

void Arret_Urgence(void *arg){

	log_task_entered();
        int android;
        
	while(1){
		//rt_printf("Thread Arret \n");

		log_sem_waiting(&var_sem_arret);
		rt_sem_p(&var_sem_arret,TM_INFINITE);
		log_sem_entered(&var_sem_arret);

                rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_android);

		android = etat_android;

		rt_mutex_release(&var_mutex_etat_android);
		log_mutex_released(&var_mutex_etat_android);
                
		rt_mutex_acquire(&var_mutex_arret, TM_INFINITE);
		log_mutex_acquired(&var_mutex_arret);

		arret = 1;

		rt_mutex_release(&var_mutex_arret);
		log_mutex_released(&var_mutex_arret);

		int err=0;
		message_stm m;
		m.label = 'a';
		m.ival = 1;
                
                err = rt_queue_write(&queue_Msg2STM,&m,sizeof(message_stm),Q_NORMAL);
                
		/*log_sem_signaled(&var_sem_envoyer);
		rt_sem_v(&var_sem_envoyer);*/

	}
}


void Envoyer(void *arg){

	rt_printf("Thread Envoyer : Debut de l'éxecution de periodique à 100 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 10000000);

	log_task_entered();

	while(1){
		//rt_printf("Thread Envoyer \n");
		rt_task_wait_period(NULL);

		message_stm m;
		int err = rt_queue_read(&queue_Msg2STM,&m,sizeof(message_stm),SECENTOP / 10000);

		if(m.label == 'c'){
			send_float_to_serial(m.fval,'c');
		}
		else if(m.label == 'a'){
			send_int_to_serial(m.ival,'a');
		}

		/*log_sem_waiting(&var_sem_envoyer);
		rt_sem_p(&var_sem_envoyer,TM_INFINITE);
		log_sem_entered(&var_sem_envoyer);

		rt_mutex_acquire(&var_mutex_consigne_couple, TM_INFINITE);
		log_mutex_acquired(&var_mutex_consigne_couple);

		send_float_to_serial(consigne_couple.consigne(),'c');

		rt_mutex_release(&var_mutex_consigne_couple);
		log_mutex_released(&var_mutex_consigne_couple);

		rt_mutex_acquire(&var_mutex_arret, TM_INFINITE);
		log_mutex_acquired(&var_mutex_arret);
		if(arret){
			send_int_to_serial(arret,'a');
		}
		rt_mutex_release(&var_mutex_arret);
		log_mutex_released(&var_mutex_arret);*/
	}
}


void Affichage(void *arg){

	unsigned char *str;
	int indice=0;
	int	bat,user,stop,com;
	float tamp1=0.0f,tamp2=0.0f,tamp3=0.0f,tamp4=0.0f;
	int sckt_gui;

	sckt_gui = init_socket(PORT_GUI);

	rt_printf("Thread Affichage : Debut de l'éxecution de periodique à 10 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 100000000);

	log_task_entered();

	while(1){
		//rt_printf("Thread Affichage \n");
		rt_task_wait_period(NULL);

		str = (unsigned char*)malloc(56* sizeof(unsigned char)); // 56 = (n * 7 ) avec n nombre de fonctions "add_info"

		rt_mutex_acquire(&var_mutex_etat_angle, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_angle);

		tamp1 = etat_angle.angle();
		tamp2 = etat_angle.vitesse_ang();

		rt_mutex_release(&var_mutex_etat_angle);
		log_mutex_released(&var_mutex_etat_angle);

		rt_mutex_acquire(&var_mutex_vitesse, TM_INFINITE);
		log_mutex_acquired(&var_mutex_vitesse);

		tamp3 = vitesse_lin.vitesse();

		rt_mutex_release(&var_mutex_vitesse);
		log_mutex_released(&var_mutex_vitesse);

		rt_mutex_acquire(&var_mutex_consigne_couple, TM_INFINITE);
		log_mutex_acquired(&var_mutex_consigne_couple);

		tamp4=consigne_couple.consigne();

		rt_mutex_release(&var_mutex_consigne_couple);
		log_mutex_released(&var_mutex_consigne_couple);

		rt_mutex_acquire(&var_mutex_batterie, TM_INFINITE);
		log_mutex_acquired(&var_mutex_batterie);

		bat = batterie.level();

		rt_mutex_release(&var_mutex_batterie);
		log_mutex_released(&var_mutex_batterie);

		rt_mutex_acquire(&var_mutex_presence_user, TM_INFINITE);
		log_mutex_acquired(&var_mutex_presence_user);

		user=presence_user;

		rt_mutex_release(&var_mutex_presence_user);
		log_mutex_released(&var_mutex_presence_user);

		rt_mutex_acquire(&var_mutex_etat_com, TM_INFINITE);
		log_mutex_acquired(&var_mutex_etat_com);

	  	com = etat_com;

		rt_mutex_release(&var_mutex_etat_com);
		log_mutex_released(&var_mutex_etat_com);

		rt_mutex_acquire(&var_mutex_arret, TM_INFINITE);
		log_mutex_acquired(&var_mutex_arret);

		stop = arret;

		rt_mutex_release(&var_mutex_arret);
		log_mutex_released(&var_mutex_arret);

 		add_info_float(str,'p',tamp1,&indice);
		add_info_float(str,'s',tamp2,&indice);
		add_info_float(str,'v',tamp3,&indice);
		add_info_float(str,'c',tamp4,&indice);

 		add_info_float(str,'b',bat,&indice);
		add_info_float(str,'u',user,&indice);
		add_info_float(str,'e',com,&indice);
		add_info_float(str,'a',stop,&indice);

		send_trame(sckt_gui, str, &indice);
    	}
}

void Communication_Android (void *arg){
    
   /* rt_printf("Thread ANDROID : Debut de l'éxecution\n");
    log_task_entered();
    
    float puissance = 0.0;
    float angle = 0.0;
    int sens = 0;
    int socket_desc;
    
    socket_desc = init_socket_server();
    
    while (1){
        
        
        
        
        update_values(socket_desc, puissance, angle, sens);
        
        rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
        log_mutex_acquired(&var_mutex_etat_android);

        etat_android = 0;

        rt_mutex_release(&var_mutex_arret);
        log_mutex_released(&var_mutex_arret);
        
        
        
    }
    */
    
    
    rt_printf("Thread ANDROID : Debut de l'éxecution de periodique à 100 Hz\n");
    rt_task_set_periodic(NULL, TM_NOW, 10000000);

    log_task_entered();
    
    float puissance = 0.0;
    float angle = 0.0;
    int socket_desc , client_sock , c , read_size, sens;
    struct sockaddr_in server , client;
    char client_message[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        //return 1;
    }
    puts("bind done");
	while (1){
    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        //return 1;
    }
    puts("Connection accepted");
	       
    rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
    log_mutex_acquired(&var_mutex_etat_android);

    etat_android = 1;

    rt_mutex_release(&var_mutex_etat_android);
    log_mutex_released(&var_mutex_etat_android);
    int MAX_SIZE = 1024;
    int len,i,taille;
    char tab[MAX_SIZE];
    char subtab[3];
    char string[MAX_SIZE];
    float data;

	/* lecture d'un message de taille max MAX_SIZE, information dans string */
 	//len = read(sckt, string, MAX_SIZE);
	while( (len = recv(client_sock , string , MAX_SIZE , 0)) > 0 ){
		taille=strlen((char*)string);
		if(taille > 0){
			memcpy(tab,string,taille);	//Si le message n'est pas vide, on copie ces informations dans tab

			for(i=0 ; i<len ; i++){
					if((tab[i] == '<')&&(tab[i+6] == '\n')){
						switch (tab[i+1]){
							//puissance
							case 'p':
									memcpy(&data,&tab[i+2],sizeof(data));
									subtab[0] = tab[i+2];
									subtab[1] = tab[i+3];
									subtab[2] = tab[i+4];
									subtab[3] = tab[i+5];
									puissance = atof(subtab); 
									break;
							//angle		
							case 'a':
									memcpy(&data,&tab[i+2],sizeof(data));
									subtab[0] = tab[i+2];
									subtab[1] = tab[i+3];
									subtab[2] = tab[i+4];
									subtab[3] = tab[i+5];
									angle = atof(subtab); 
									break;
							//sens
							case 's':
									//puts("S \n");
									memcpy(&data,&tab[i+2],sizeof(data));
									subtab[0] = tab[i+2];
									subtab[1] = tab[i+3];
									subtab[2] = tab[i+4];
									subtab[3] = tab[i+5];
									sens = atoi(subtab);
									break;

						}//case
					}//<\n
			} //for
		} //if taille
		rt_printf("Puissance : %f \nAngle : %f \nSens : %d \n", puissance, angle, sens);
                //puissance = k1*30*puissance;
                rt_mutex_acquire(&var_mutex_consigne_couple, TM_INFINITE);
                log_mutex_acquired(&var_mutex_consigne_couple);

                consigne_couple.set_consigne(puissance*10*0.80435);

                rt_mutex_release(&var_mutex_consigne_couple);
                log_mutex_released(&var_mutex_consigne_couple);
	} //while 
        
        if(read_size == 0)
        {
            puts("Client disconnected");
            rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
            log_mutex_acquired(&var_mutex_etat_android);

            etat_android = 0;

            rt_mutex_release(&var_mutex_etat_android);
            log_mutex_released(&var_mutex_etat_android);
            fflush(stdout);
        }
        else if(read_size == -1)
        {
            perror("recv failed");
            rt_mutex_acquire(&var_mutex_etat_android, TM_INFINITE);
            log_mutex_acquired(&var_mutex_etat_android);

            etat_android = 0;

            rt_mutex_release(&var_mutex_etat_android);
            log_mutex_released(&var_mutex_etat_android);
        }
    }

}
    
        
     
    



