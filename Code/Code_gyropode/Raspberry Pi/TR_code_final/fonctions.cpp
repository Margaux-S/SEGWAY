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
	int com;
	int uart0_filestream;

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
				

				
                                
                                rt_mutex_acquire(&var_mutex_consigne_couple, TM_INFINITE);
                                log_mutex_acquired(&var_mutex_consigne_couple);

                                c = (k1 * angle + k2 * vit_angulaire);
                                consigne_couple.set_consigne(c);

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
                                m.fval = c;
                                if (not(android)){
                                    err = rt_queue_write(&queue_Msg2STM,&m,sizeof(message_stm),Q_NORMAL);
                                }
				//rt_sem_v(&var_sem_envoyer);
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
        
	while(1){
		//rt_printf("Thread Arret \n");

		log_sem_waiting(&var_sem_arret);
		rt_sem_p(&var_sem_arret,TM_INFINITE);
		log_sem_entered(&var_sem_arret);

		rt_mutex_acquire(&var_mutex_arret, TM_INFINITE);
		log_mutex_acquired(&var_mutex_arret);

		arret = 1;

		rt_mutex_release(&var_mutex_arret);
		log_mutex_released(&var_mutex_arret);

		int err=0;
		message_stm m;
		m.label = 'a';
		m.ival = 1;
                
                if (not(android)){
                    err = rt_queue_write(&queue_Msg2STM,&m,sizeof(message_stm),Q_NORMAL);
                }
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
                        //rt_printf("J'envoie une consigne %f au STM32 \n", m.fval);
		}
		else if(m.label == 'a'){
			send_int_to_serial(m.ival,'a');
                        //rt_printf("J'envoie arrêt au STM32 \n");
		} else if (m.label == 'd'){
                    send_float_to_serial(m.fval,'d');
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
	rt_printf("Thread Affichage : Debut de l'éxecution de periodique à 100 Hz\n");
	rt_task_set_periodic(NULL, TM_NOW, 10000000);

	log_task_entered();

rt_printf("8");
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
    
    rt_printf("Thread ANDROID : Debut de l'éxecution\n");
    log_task_entered();
    float puissance = 0.0;
    float angle = 0.0;
    int socket_desc , client_sock , c , read_size, sens;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        rt_printf("Could not create socket\n");
    } else {
        rt_printf("Socket created\n");
    }
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        rt_printf("bind failed. Error\n");
    } else{
        rt_printf("bind done\n");
    }
    while (1){
        android = 0;
          
        //Listen
        listen(socket_desc , 1);

        //Accept and incoming connection
        rt_printf("Waiting for incoming connections...\n");
        c = sizeof(struct sockaddr_in);

        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            rt_printf("accept failed\n");
        } else {
            rt_printf("Connection accepted\n");
        }
        android = 1;

       
        int MAX_SIZE = 100;
        int i, len;
        int noerror = 0;
        char tab[MAX_SIZE];
        char subtab[4];
        char senstab[2];
        char string[MAX_SIZE];
            /* lecture d'un message de taille max MAX_SIZE, information dans string */
            while( (len = recv(client_sock , string , MAX_SIZE , 0)) > 0 ){
                    read_size=strlen((char*)string);
                    if(read_size > 0){
                        memcpy(tab,string,read_size);	//Si le message n'est pas vide, on copie ces informations dans tab
                        for(i=0 ; i<(read_size-1) ; i++){
                            if(tab[i] == '<'){
                                switch (tab[i+1]){
                                    //puissance
                                    case 'p':
                                            subtab[0] =  tab[i+2];
                                            subtab[1] =  tab[i+3];
                                            subtab[2] =  tab[i+4];
                                            subtab[3] =  tab[i+5];
                                            puissance = atof(subtab);
                                            break;
                                    //angle		
                                    case 'a':
                                            subtab[0] =  tab[i+2];
                                            subtab[1] =  tab[i+3];
                                            subtab[2] =  tab[i+4];
                                            subtab[3] =  tab[i+5];
                                            angle = atof(subtab); 
                                            break;
                                    //sens
                                    case 's':

                                            senstab[0] = tab[i+2];
                                            sens = atoi(senstab);
                                            break;
                            }//case
                        } //<\n                    
                    } //for
                        if (!sens){
                            puissance = -puissance;
                        }
                    float c = puissance*6;
                    int err=0;
                    
                    message_stm m;
                    m.label = 'c';
                    if (noerror){
                        noerror = 0;
                        m.fval = c+0.1;
                    } else {
                        noerror = 1;
                        m.fval = c;
                    }
                    err = rt_queue_write(&queue_Msg2STM,&m,sizeof(message_stm),Q_NORMAL);
                    
                    message_stm d;
                    d.label = 'd';
                    d.fval = angle;
                    err = rt_queue_write(&queue_Msg2STM,&d,sizeof(message_stm),Q_NORMAL);
                } //if taille*/    
            } //while 
            if(len <= 0)
            {
                android = 0;
                rt_printf("Client disconnected\n");
            }
            else if(len == -1)
            {
                android = 0;
                rt_printf("recv failed\n");
            }
            
        }
}