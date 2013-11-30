//procKeeper.c
#include "process.h"
#include <stdio.h>
const char * conf = "/etc/proclist.ini";

int main(int argc, char * argv[])
{
	struct proc_struct current_proc_list[1024], config_proc_list[1024];
	int  current_run_size, config_run_size; 
	int i = 0;
	config_run_size = get_proc_list_from_file(config_proc_list, 1024, conf);
	
	if(config_run_size <= 0)
	{
		return 0;
	}
	//printf("%d\n", must_run_size);
	//for(i = 0; i < must_run_size; i++)
	//print_proc(&(must_run_proc_list[i]), 1);
	printf ("%s:running now...\n", argv[0]);
	while(1)
	{
		int i = 0;
		int size = get_proc_list(current_proc_list, 1024); 

		for(i = 0; i < config_run_size; i++)
		{
			//do not find config task in current task list
			if(proc_find(&(config_proc_list[i]), current_proc_list, size) < 0)
			{
				// it was runnig last time
				if(config_proc_list[i].new_status == 1)
				{
					config_proc_list[i].new_status = 0;
					config_proc_list[i].dead_time = 0;	
				}
				//it was not running last time
				if(config_proc_list[i].dead_counter >= 0)
				{
					config_proc_list[i].dead_time += 1;
					if(config_proc_list[i].dead_time >= config_proc_list[i].dead_counter)
					{
						//restart process	
						startProc(&(config_proc_list[i]));
						config_proc_list[i].dead_time = 0;
					}	
				}
				
				//print_proc(&(config_proc_list[i]), 1);	
			}
			else
			{
				//print_proc(&(config_proc_list[i]), 1);
				config_proc_list[i].new_status = 1;
				config_proc_list[i].dead_time = 0;
				if(config_proc_list[i].live_counter >= 0)
				{
					config_proc_list[i].live_time += 1;
					if(config_proc_list[i].live_time > config_proc_list[i].live_counter)
					{
						killProc(&(config_proc_list[i]));
						config_proc_list[i].live_time = 0;
					}
				}
			}	
		}
		
		sleep(1);
	}
}

