#ifndef _PUBDEF_H_
#define _PUBDEF_H_
#include "NetHelper.h"
//----it was copied from cmtools/constdef.h --start@2011-07-09 by duanjigang
#define MSG_TYPE_STAT 1
#define MSG_TYPE_CMD 2
#define MSG_TYPE_FILE 3
#define PROXY_LEN 128
//----it was copied from cmtools/constdef.h --finish@2011-07-09 by duanjigang

//define for host result value --start
#define RET_ICE_ERROR  1
#define RET_CONNECT_FAILED  2
#define RET_CALL_FAILED   3
#define RET_SEND_FAILED   4
#define RET_STORE_FAILED   5
#define RET_SUCCESS  0
//define for host result value --finish


//define for device
#define HOST_NAME 100
typedef struct
{
	unsigned int 	host_id; //host id
	unsigned int 	host_addr; //host address
	unsigned int 	host_group;//group id
	char		host_name[HOST_NAME]; //host name
}device_t;
#endif
