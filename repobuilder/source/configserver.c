#include "configserver.h"
#include "inihelper.h"
#include "nethelper.h"
#include <sys/stat.h>
#include <sys/types.h>
extern config_t g_config;
int 	mkdir_rec(const char* path)
{
	char szpath[512] = {0};
	int i = 0;
	int len = 0;
	sprintf (szpath, "%s", path);
	len = strlen(szpath);
	for (i = 1; i < len; i++)
	{
		if (szpath[i] == '/')
		{
			szpath[i] = '\0';
			mkdir (szpath, 0777);
			szpath[i] = '/';
		}
	}
	mkdir (szpath, 0777);
	return 1;
}

void InitArgs(int argc, char* argv[])
{
	if (argc > 1)
	{
		if (strncasecmp(argv[1], "-v", 2) == 0)
		{
			printf("%s: v1.2 2011-03-24 version list:\n", argv[0]);
			printf("\tv1.2: 2011-03-24:creating \n");
			exit(0);
				
		}
	}
}
/*
void error_info (int nError, char * szbuf)
{
	switch (nError)
	{
		case RET_SUCCESS: 
				sprintf (szbuf, "success");break;
		case RET_CONNECT_FAILED: 
				sprintf (szbuf, "connect host failed");break;
		case RET_CALL_FAILED: 
				sprintf (szbuf, "run rpc failed");break;
		case RET_SEND_FAILED: 
				sprintf (szbuf, "send data failed");break;
		case RET_STORE_FAILED: 
				sprintf (szbuf, "store data to local file failed");break;

		default: 
				sprintf (szbuf, "unknow error value %d", nError);break;
	}
}
*/

int repo_build_func (void*data);
//delivery application to each thread
//because there may be so many host to send a file or notify
//a message, we just divid the host list to several group, and 
//each group is served by a thread
param_t g_plist[MAX_THREAD];
//app_t   g_app_list[MAX_THREAD];

//int	deliver_app_task (app_t * pa, thread_pool_t * pool, config_t * conf)
int	deliver_app_task (thread_pool_t * pool, config_t * conf)
{
	unsigned int index = 0;	
	
	for (index = 0; index < pool->thread_num; index++)
	{
		g_plist[index].func = repo_build_func;					
		g_plist[index].data = NULL;
		g_plist[index].conf = conf;
		g_plist[index].index = index+1;
		pool->thread_list[index].param = &g_plist[index];
	}
	//loop repo list of config
	for (index = 0; index < conf->repo_num; index++)
	{
			repo_t * pr = conf->repo_list + index;
			if (g_plist[index % pool->thread_num].data == NULL )
				g_plist[index % pool->thread_num].data = pr;
			else
			{
				pr->next = g_plist[index % pool->thread_num].data;
				g_plist[index % pool->thread_num].data = pr;
			}
	}
	return 1;
}

int repo_build_func (void*data)
{
	param_t * pa = (param_t*)data;
	repo_t * pr = NULL;
	config_t * conf  = (config_t*)pa->conf;

	if (!pa)
	{
		printf ("%s-%d:invalid parameter data\n", __FILE__, __LINE__);
		return -1;
	}
	repo_t * rp = (repo_t*)pa->data;
	if (!rp)
	{
		printf ("invalid repo list for thread:%d\n", pa->index);
		return -1;
	}
	
	//loop
	while (1)
	{
		sleep (1);
		pr = rp;
		while (pr)
		{
				time_t ct = time (0);
				//time coming or signal file exists.
				if ( (ct >= pr->last_update + pr->intv) || (access(pr->sigfile, F_OK) == 0))
				{
					struct stat st;
					unlink (pr->sigfile);
					printf ("%s:time for thread_%03d to update <path=%s,interval=%d>\n",
					get_time_str(NULL), 
										
					pa->index, pr->path, pr->intv);

					if (!stat(pr->path, &st))
					{
							if (S_ISDIR(st.st_mode))
							{
								char cmd[512] = {0};
								//for createrepo-0.4.11-3.el5
								sprintf (cmd, "/usr/bin/createrepo -d -p --update -o %s %s", pr->path, pr->path);
								//for createrepo-0.4.4-2
								//sprintf (cmd, "/usr/bin/createrepo  -p  -o %s %s", pr->path, pr->path);
								printf ("Run commad:"YELLOW"%s"NONE"\n", cmd);
								system(cmd);
							}else
							{
								if(conf->verb)
								printf (""RED"%s"NONE" is not a directory\n", pr->path);
							}	
					}else
					{
						if(conf->verb)
						printf ("stat "RED"%s"NONE" failed\n", pr->path);
					}

					pr->last_update = time(0);
				}
				pr=pr->next;
				usleep(100);
		}
	}
	return 1;
}
//start each thread
int	start_thread(thread_pool_t * pool, const config_t * conf)
{
	unsigned int index = 0;
	for (index = 0; index < pool->thread_num; index++)	
	{
		pool->thread_list[index].status = status_ready;		
		if (conf->verb)	printf ("starting thread "GREEN"%03d"NONE"\n",pool->thread_list[index].index);
	}
	return pool->thread_num;
}


int	init_config (config_t * config, int argc, char* argv[])
{
	int c = 0;
	const char* szuse = "\t-f\tconfigfile\n\t-d\t[daemon]\n\t-v\t[verbose]\n";
	memset (config, 0, sizeof(config_t));
	config->daemon = 0;
	config->verb = 0;
	while ((c = getopt (argc, argv, "f:dv")) != -1)
	{
		switch (c)
		{
			//config file
			case 'f':
				strcpy(config->conf, optarg);
				break;
			//daemon
			case 'd':
				config->daemon = 1;
				break;
			//verb
			case 'v':
				config->verb = 1;
				break;

			default:	
					printf ("usage:%s\n%s\n", argv[0], szuse);
			break;
		}
	}
	if (!strlen(config->conf))
	{
		printf ("specify a command with '-c command'\n");
		printf ("usage:%s\n%s\n", argv[0], szuse);
		return -1;
	}
	if (access (config->conf, R_OK))
	{
		printf ("access file '%s' failed\n", config->conf);
		return -1;
	}
	return 1;
}
char* get_time_str(time_t * tm)
{
	struct tm * newtime;
	static char s[32]= {0};
 	if(tm == NULL)
	{
		time_t ct;
        	time(&ct);
        	newtime = gmtime(&ct);
	}else
	{
		newtime = gmtime(tm);
	}
	sprintf( s,"%04d-%02d-%02d %02d:%02d:%02d", 
	newtime->tm_year + 1900, 
				(newtime->tm_mon + 1)%12, 
				newtime->tm_mday,
         		(newtime->tm_hour + 8)%24, 
				newtime->tm_min, newtime->tm_sec );
		return s;
}

int load_repo_conf (config_t * conf)
{
	void * ini = NULL;
	section sc;
	if(NULL == (ini = init_ini(conf->conf)))
     {
			printf ("%s-%d:open file '%s' for reading failed\n",__FILE__, __LINE__, conf->conf);
            return -1;
     }
	
	conf->interval = atoi (read_string(ini, "general", "interval", "600"));
	conf->nthread = atoi (read_string(ini, "general", "thread_num", "10"));
	sprintf (conf->sigfile, "%s", read_string(ini, "general", "signalfile", "update.txt"));

	if (conf->verb)
	{
		printf ("repobuilder conf:\n\tinterval = "YELLOW"%d"NONE"\n\tthread_num = "YELLOW"%d"NONE"\n\tsignalfile="YELLOW"%s"NONE"\n", 
		conf->interval, conf->nthread, conf->sigfile);
	}
	//reading each repo
	conf->repo_num = 0;
	while (read_section(ini, &sc))
    {   
        int i = 0;
		repo_t rp;
		int on = 1;
		int valid = 1;
		rp.intv = conf->interval;
		if (strncmp(sc.title, "repo", 4))continue;
        for ( i = 0; i < sc.entry_number; i++)
           {
                  if (strncasecmp(sc.entry_list[i].var_name, "path", 4) == 0)
                    {
                           strcpy(rp.path, sc.entry_list[i].var_value);
                           continue;
                     }	

					if (strncasecmp(sc.entry_list[i].var_name, "on", 2) == 0)
                    {
                           	on = atoi(sc.entry_list[i].var_value);
							if (!on)
							{
								valid = 0;
								break;
							}
                           continue;
                     }	

					if (strncasecmp(sc.entry_list[i].var_name, "interval", 8) == 0)
                    {
							int intv = 0;
                           	intv = atoi(sc.entry_list[i].var_value);
							if (intv > 0) rp.intv = intv;
                           	continue;
                     }	


			}//end of for
			if (!valid )
			{
				if (conf->verb) printf ("just ignore "YELLOW"'%s'"NONE"..\n", rp.path);
				continue;
			}


			sprintf (rp.sigfile, "%s/%s", rp.path, conf->sigfile);
			memcpy (conf->repo_list + conf->repo_num, &rp, sizeof(repo_t));
			conf->repo_list[conf->repo_num].next = NULL;
			conf->repo_list[conf->repo_num].last_update = 0;
						if (conf->verb)
			{
				printf ("load repo conf:\tpath=\t"YELLOW"%s"NONE"\tinterval=\t"YELLOW"%d"NONE"\tsigfile="YELLOW"%s"NONE"\n",
					rp.path,rp.intv,rp.sigfile);
			}

			conf->repo_num++;
	}//end of while read section
	close_ini (ini);
	if (conf->verb)
	{
		printf ("totally "YELLOW"%d"NONE" repo loaded\n", conf->repo_num);
	}
	if (conf->nthread > conf->repo_num) conf->nthread = conf->repo_num;
	return conf->repo_num;
}
