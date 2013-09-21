#ifndef _NT_HELPER_H_
#define _NT_HELPER_H_ 

#ifdef _WIN32
#include <Winsock2.h>
#include <Winbase.h>
#pragma comment( lib, "ws2_32.lib" ) 
#endif
#include <stdio.h>
#include "os_independent.h"
#include <time.h>
#include <ctype.h>

#ifdef __LINUX__
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
typedef struct
	{
		unsigned int ip;
		unsigned int netmask;
		unsigned char mac[6];
		char	name[30];
	}nic_t;

extern int	get_nic_list(nic_t *list);
#endif
