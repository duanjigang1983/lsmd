#ifndef _CONFIG_SERVER_H_
#define _CONFIG_SERVER_H_
#define EXE_NAME "cmclient"
#ifdef _WIN32
#include <wtypes.h>
#include <Windows.h>
#define SLEEP_SEC(t) Sleep(t)
#else
#define SLEEP_SEC(t) sleep(t)
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#define FILE_NAME_LEN 64
#define MAX_ICE_STRING 64

#include <signal.h>
#include "datadef.h"
#include "ColorDefine.h"

#define STR_IP(str, n) sprintf((str), "%u.%u.%u.%u", (n)  & 0xff, ((n)>> 8) & 0xff, ((n)>>16) & 0xff, ((n)>>24) & 0xff)
#define PATH_LEN 256


extern  unsigned char	run_over(thread_pool_t * pool);
extern  void	show_app(const app_t *);
extern	int 	init_thread_slot(thread_pool_t*, const config_t *);
extern	int		deliver_app_task (app_t * pa, thread_pool_t * pool, config_t * conf);
extern  int		start_thread(thread_pool_t * pool, const config_t *);
extern  int		get_result (app_t* pa, const config_t * config);
extern	int		reset_thread (thread_pool_t * pool);
extern	int		terminate_thread (thread_pool_t * pool);
extern  int		init_config (config_t * config, int argc, char* argv[]);
extern  int		send_cmd2host (dev_task_t* dev, const app_t * pa, const config_t * config);
extern	int 	init_tcp_sock(int	aMode, const char *aAddr, unsigned short  aPort);
extern	int 	tcp_send(int aFd,char *aBuf,int aLen);
extern	int 	tcp_read(int aFd,char *aBuf);
extern	int 	tcp_connect(int fd, const char *aAddr, unsigned short aPort);
extern	int 	tcp_close(int aFd);
extern	int		show_progress (app_t* pa);
extern	char* 	get_time_str(time_t * tm);
extern 	int 	tcp_connect_timeout ( char *host, unsigned short int port, unsigned int timeout );
extern	int		daemon (int nochdir, int noclose);
extern	int 	load_repo_conf (config_t * conf);
#endif

