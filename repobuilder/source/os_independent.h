#ifndef _OS_INDEP_H_
#define _OS_INDEP_H_
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#include <Winsock.h>
#include <Winbase.h>
#include <WTYPES.H>
#pragma comment(lib, "Ws2_32.lib")
#define CLOSE_FILE(fd) _close(fd)
#define CLOSE(s) closesocket(s);
typedef int socklen_t;
typedef int key_t;
typedef HANDLE    mem_file_handle;
#define UNLINK(f) _unlink(f)
static _int64 atoll(const char *string){return _atoi64(string);} 
static int tt = 1;
#endif

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
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
typedef void*    mem_file_handle;
#define CLOSE(s) close(s)
#define UNLINK(f) unlink(f)
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x4000
#endif
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x40
#endif
#define STR_IP(str, n) sprintf((str), "%u.%u.%u.%u", (n)  & 0xff, ((n)>> 8) & 0xff, ((n)>>16) & 0xff, ((n)>>24) & 0xff)


#define MAX_VAL(a, b) ((a) >= (b)) ? (a) : (b)
#define MIN_VAL(a, b) ((a) <= (b)) ? (a) : (b)

#endif
