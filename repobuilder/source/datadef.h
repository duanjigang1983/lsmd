#ifndef _DATA_DEF_H_
#define _DATA_DEF_H_
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
typedef int (*func_t)(void*data);

typedef struct 
{
	func_t func;
	void* data;	
	int index;
	void * conf;
}param_t;


enum thread_status
{
	status_free = 1,
	status_ready = 2,
	status_running = 3,
	status_over = 4,
	status_dead = 5,
	status_already_dead = 6 
};
typedef struct
{
	pthread_t thread; //thread
	pthread_mutex_t mutex; //mutex
	unsigned char status; //status
	unsigned int  index;
	param_t *     param;
}thread_t;
#define MAX_THREAD 100
typedef struct
{
	thread_t thread_list[MAX_THREAD];
	unsigned int thread_num;
}thread_pool_t;

//config information
#define MAX_NIC 16
#define CMD_LEN 256
#define THREAD_NUM 100
#define TIMEOUT 5
//#define DATA_DIR "data"
#define MAX_REPO 100
typedef struct _repo
{
		char path [256];
		char sigfile[256];
		int intv;
		void * next;
		time_t last_update;
}repo_t;

typedef struct
{
	unsigned short  nthread;
	unsigned char 	daemon;
	unsigned char 	verb;
	char			conf[256];
	char 			sigfile[64];
 	int 			interval;
	repo_t			repo_list[MAX_REPO];
	int 			repo_num;
}config_t;

#endif
