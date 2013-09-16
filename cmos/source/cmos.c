#include "cmos.h"

int main(int argc, char* argv[])
{
	char szline[1024] = {0};
	u_int32_t nRet = RET_OK;
	u_int32_t nExit = 0;
	//used to store status of cmos
	cmos_status_t stat;
	cmos_config_t cfg, old_cfg;
	
	cm_parseparam (argc, argv, &cfg);	

	if (RET_OK != init_status(&stat, &cfg))
	{
		printf ("init status cmos failed\n");
		return 1;
	}
	#ifdef _USE_READLINE_	
	init_readline (&stat, &cfg);
	#endif
	if (RET_OK != init_signal(&stat))
	{
		printf ("init signal cmos failed\n");
		return 1;
	}
	
	load_plugins (stat.current_dir, &stat, &cfg);
	#ifdef _USE_READLINE_
	sort_completion ();
	#endif
	//printf ("%s", INPUT_FLAG);
	//printf ("%s%s", stat.show_path, INPUT_FLAG);
	

	//while (fgets(szline, 1024, stdin))
	memcpy (&old_cfg, &cfg, sizeof(cfg));
	
	while (read_line(szline, 1024, &cfg) > 0)
	{
		char * p = strchr (szline, '\r');
		if (p) *p = '\0';
		p = strchr (szline, '\n');
		if (p) *p = '\0'; 
		nExit = !old_cfg.keep_alive;
		cfg.quit_flag = 0;
		if (strlen(szline) == 0) goto LOOP;
		nRet = handle_message (szline, &stat, &cfg);
		//fprintf (stderr, "CMD:%s\n", szline);
		switch (nRet)
		{
			case RET_EXIT:
			{
				//do some cleaning work and then eixt
				nExit = 1;
				break;
			}
			//access dir failed
			case RET_ACCESS_FAILED:
			{
				//printf ("access failed\n");
				break;
			}
			//it might be a bug
			case RET_BUG:
			{
				printf ("it might be a bug!!\n");
				init_status(&stat, &cfg);
				break;
			}
			default:
			{
				break;
			}
		}
		

LOOP:
		usleep (10);//added by jigang.djg@2011-10-19 15:51
				//config changed
		//if (memcmp (&old_cfg, &cfg, sizeof(cfg ) - sizeof(u_int32_t) ))
		if (memcmp (&old_cfg, &cfg, sizeof(cfg )))
		{
			memcpy (&old_cfg, &cfg, sizeof(cfg));
		}
		
		//nExit = (!cfg.keep_alive)||(cfg.quit_flag);
		
		nExit = !cfg.keep_alive || cfg.quit_flag;
		//if (!nExit) nExit = cfg.quit_flag;
		if (nExit)
		{
			 break;
		}

		memset (szline, 0, strlen(szline));

		fflush(stdin);
		fflush(stdout);	

	}
	if (stat.cmd_list)
	{
		free (stat.cmd_list);
	}
	//fprintf (stderr, "cmos exiting now\n");
	return nRet;
}
