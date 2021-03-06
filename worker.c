  
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include<stdio.h>
#include<string.h>
#include"info.h"
#include<endian.h>
#include<stdint.h>
#include <sys/time.h>
#include <sys/resource.h>



#ifndef _worker_c
#define _worker_c

/* Gestisce la comunicazione tra client e server, calcola la stima e se è consistente
la manda al supervisor*/
void* worker (void*arg) {

    struct timeval t;
    uint64_t buffid=0, id=0;
	int j=0, n=0;
	int* temp = (int*) arg;
	int fd = temp[0];
	int i = temp[1];
	int fstim=0,min=999999,tempstima=0;
	
	while ((n = read(fd, &buffid, sizeof(uint64_t))) == sizeof(uint64_t)) {
		gettimeofday(&t,NULL);
		if (j==0){
            id=be64toh(buffid);
			tempstima=((t.tv_sec)*1000+(t.tv_usec/1000));
        }
		if(j!=0){
			fstim=((t.tv_sec)*1000+(t.tv_usec/1000));
			int lol=fstim-tempstima;
			if(lol>0 && lol<min){
				min=lol;
				tempstima=fstim;
			}
		}
		fprintf(stdout,"SERVER %d INCOMING FROM %lx@%d\n",(i+1),id,tempstima);
		j++;
	}
	
	close(fd);

	if (id!=0 && min>0) {
		fprintf(stdout,"SERVER %d CLOSING %lx ESTIMATE %d\n", (i+1), id, min);
		msg m = {id,min};
		CHECKLOCK1(pthread_mutex_lock(&lock));
		write(apipe[i][1], &m, sizeof(msg));
		CHECKLOCK1(pthread_mutex_unlock(&lock));
	}

	pthread_exit(NULL);

	
}

#endif
