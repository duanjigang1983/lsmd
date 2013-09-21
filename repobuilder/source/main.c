#include <stdio.h>
#include "datadef.h"
#include "configserver.h"
#include <string.h>
#include "datadef.h"

config_t g_config;

int main (int argc, char* argv[])
{
	thread_pool_t g_thread_pool;

	//load config information
	if (init_config (&g_config, argc, argv) <= 0)
	{
		return 1;
	}
	if (load_repo_conf (&g_config) <= 0 )
	{
		return 1;
	}
	if (g_config.daemon)
	{
		daemon (0, 0);
	}

	//init thread slot
	g_thread_pool.thread_num = g_config.nthread;
	//too much thread to create
	
	if (init_thread_slot (&g_thread_pool, &g_config) <= 0)
	{
		return 1;
	}
	
	//deliver task to each thread
	deliver_app_task (&g_thread_pool, &g_config);		
	//start run each application
	start_thread (&g_thread_pool, &g_config);	
	//check status whether all thread run over
	while (!run_over(&g_thread_pool))
	{
		sleep (3);
	}
	reset_thread (&g_thread_pool);
	terminate_thread (&g_thread_pool);
	printf ("exiting.............\n");
	return 1;
}
