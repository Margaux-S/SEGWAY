/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../inc/lib_android.h"

void read_socket_values(int sckt, int client_sckt, float puissance, float angle, int sens){
	int MAX_SIZE = 1024;
	int len,i,taille;
	char tab[MAX_SIZE];
	char subtab[3];
	char string[MAX_SIZE];
	float data;

	/* lecture d'un message de taille max MAX_SIZE, information dans string */
 	//len = read(sckt, string, MAX_SIZE);
	while( (len = recv(client_sckt , string , MAX_SIZE , 0)) > 0 ){
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
		printf("Puissance : %f \nAngle : %f \nSens : %d \n", puissance, angle, sens);
	} //while 
}





int init_socket_server()
{
    int socket_desc ;
    struct sockaddr_in server;

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
        return 1;
    }
    puts("bind done");
    

    return 0;
}

void update_values(int socket_desc, float puissance, float angle, int sens) {
    int client_sock, read_size;
    struct client;
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
    }
    puts("Connection accepted");

    read_socket_values(socket_desc, client_sock, puissance, angle, sens);
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
}








