/* 
 * File:   includes.h
 * Author: INSA Toulouse
 *
 * Created on 20 May 2017
 */



#ifndef INCLUDES_H
#define	INCLUDES_H

#include <stdio.h>
#include <stdlib.h>     
#include <assert.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/mutex.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>
#include <alchemy/queue.h>
#include <alchemy/pipe.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>


#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		        //Used for UART

#include <time.h>
#include "global.h"
#include "fonctions.h"
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <errno.h>

#include "lib/lib.h"
#endif	/* INCLUDES_H */
