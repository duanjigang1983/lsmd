#include "cmos.h"

static void	sig_func(int signo)
{
	//printf ("cmos exiting now, bye world\n");
	exit(1);
}
u_int8_t	init_signal(const cmos_status_t * stat)
{
	struct sigaction old, new;

	memset(&new, 0, sizeof(new));

	new.sa_handler = sig_func;

	new.sa_flags |= SA_RESTART;
	//new.sa_flags |= SA_NOMASK;

	if (sigaction(SIGTERM, &new, &old) < 0)
	{
		printf("cmos:");
		return RET_SIGNAL_ERR;
	}	

	if (sigaction(SIGINT, &new, &old) < 0)
	{
		printf("cmos:");
		return RET_SIGNAL_ERR;
	}	

	if (sigaction(SIGALRM, &new, &old) < 0)
	{
		printf("cmos: alarm");
		return RET_SIGNAL_ERR;
	}	
	return RET_OK;
}


