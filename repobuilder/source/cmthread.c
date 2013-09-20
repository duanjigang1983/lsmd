#include "configserver.h"
#include "datadef.h"

/*
application thread used to run a comman task passd by external thread
*/
void * app_thread(void*data)
{
	thread_t * pt = (thread_t*)data;
	pthread_detach (pthread_self());

	while (pt->status != status_dead)
	{
		//when it is free, do nothing...
		if (pt->status == status_free)
		{
			usleep (10);
			continue;
		}
	
		//ready means it ought to do something,free -> ready is changed by outter thread
		//but not itself
		if (pt->status == status_ready)
		{
			//printf ("thread_%d: ready....go !!!\n", pt->index);
			pt->status = status_running;
			continue;
		}

		//running, do something
		if (pt->status == status_running)
		{
			if (!pt->param)
			{
				printf ("error parameter for thread %u\n", pt->index);
				goto OVER;
			}
			
			if (pt->param->func && pt->param)
			{
				pt->param->func (pt->param);
			}else
			{
				printf ("error paramter (func,data) for thread %u\n",pt->index);
			}		
			OVER:
			//when everything is done, change status to status_over myself
			pt->status = status_over;
			//printf ("thread_%d: run over\n", pt->index);
			continue;
		}

		//when i am running over, just waitting someone else to change the 
		//status to status_ready to run new tasks  or status_dead to exit
		if (pt->status == status_over)
		{
			usleep(10);
			continue;
		}

		//it's time to say goodbye...
		if (pt->status == status_dead)
		{
			printf ("good luck, i'm now exiting..bye\n");
			break;
		}
	}
	pt->status = status_already_dead;
	pthread_exit (0);
}


int 	init_thread_slot(thread_pool_t * pp, const config_t * conf)
{
	unsigned int index = 0;
	
	if (!pp)
	{
		printf ("error thread_pool pointer, program bug!!!");
		exit (0);
	}
	if ((pp->thread_num <=0) || (pp->thread_num > MAX_THREAD))
	{
		printf ("error thread number, default value is used\n");
		pp->thread_num = MAX_THREAD - 1;
	}

	//init thread parameters
	for ( index = 0; index < pp->thread_num; index++)
	{
		pp->thread_list[index].status = status_free;
		pp->thread_list[index].index = index;
		pp->thread_list[index].param = 0;
	
		//init mutex	
		if(pthread_mutex_init (&(pp->thread_list[index].mutex), 0))
		{
			printf ("pthread_mutex_init for thread_%u failed\n", index);
			return -1;
		}

		//create thread
		if (pthread_create (
			&(pp->thread_list[index].thread), //thread entry
			0, //attribute
			app_thread, //start function
			pp->thread_list + index))//parameter passed to thread function
		{
			printf ("create thread for thread %u failed\n", index);
			return -1;
		}else
		{
			if (conf->verb)
			printf ("create thread "GREEN"%03d"NONE" success\n", index + 1);
		}
	}
	return 1;
}
unsigned char	run_over(thread_pool_t * pool)
{
	unsigned char bRet = 1;
	unsigned int index = 0;
	
	for (index = 0; index < pool->thread_num; index++)
	{
		if (pool->thread_list[index].status != status_over)
		{
			bRet = 0;
			break;
		}
	} 
	return bRet;
}

int	reset_thread (thread_pool_t * pool)
{
	int nRet = 0;
	unsigned int index = 0;
	for (index = 0; index < pool->thread_num; index++)
	{
		if (pool->thread_list[index].status == status_over)
		{
			pool->thread_list[index].status = status_free;
			pool->thread_list[index].param = 0;
			nRet++;
		}	
	}
	return nRet;
}

int	terminate_thread (thread_pool_t * pool)
{
	int nRet = 0;
	unsigned int index = 0;
	for (index = 0; index < pool->thread_num; index++)
	{
		time_t start = time (0);
		pool->thread_list[index].status = status_dead;
		while (pool->thread_list[index].status != status_already_dead)
		{
			usleep (10);
			if (time(0) - start >= 5)
			{
				printf ("waitting for thread_%u to exit timeout\n", 
				pool->thread_list[index].index);
				break;
			}
		}
		nRet++;
	}
	return nRet;
}
