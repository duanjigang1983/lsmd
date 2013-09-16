#include "cmos.h"
int32_t makeargv(const char *s, char ***argvp)
{
        const char *snew;
        const char *pch;
        size_t size;
        char *t;
        char *pnext;
        char quote = 0;
        int32_t numtokens;
        int32_t i;

        if ((s == NULL) || (argvp == NULL))
        {
		//printf ("%s-%d\n", __FILE__, __LINE__);
                errno = EINVAL;
                return -1;
        }

        /* snew is real start of the string, tricky? */
        snew = s + strspn(s, " \t");
        size = strlen(snew) + 1;

        if ((t = malloc(size)) == NULL)
	{
		//printf ("%s-%d\n", __FILE__, __LINE__);
                return -1;
	}
	for (pch = s, pnext = t, numtokens = 1; *pch; pch++)
        {
                switch (*pch) 
		{
                	case '\\':
                        	++pch;
                        	if (*pch == '\0') break;
                        		switch (*pch) 
					{
                                		default:
                                		*pnext++ = *pch;
                                		break;
                                        }
                         	break;
                	case '"':
                	case '\'':
                        	quote = quote ? '\0' : *pch;
                        	break;
                        case ' ':
                        case '\t':
                        
				if (quote) 
				{
                        		*pnext++ = *pch;
         			} else
				{
          				// skip continuing white-space 
          				if (pnext > t && *(pnext - 1) != '\0') 
					{
            					*pnext++ = '\0';
            					++numtokens;
          				}
        			}
        		break;
      			default:
        			*pnext++ = *pch;
        		break;
    		}
		*pnext = '\0';
  	}

	if (quote) 
	{
		
		//printf ("%s-%d\n", __FILE__, __LINE__);
    		return -1;
  	}

  	// create argument array for ptrs to the tokens 
  	if ((*argvp = malloc((numtokens + 1) * sizeof(char *))) == NULL) 
	{
   		int error = errno;
    		free(t);
    		errno = error;
		//printf ("%s-%d\n", __FILE__, __LINE__);
    		return -1;
  	}

  	
	for (i = 0, pnext = t; i < numtokens; i++, pnext = pnext + strlen(pnext) + 1)
    	{
		*((*argvp) + i) = pnext;
  	}
	*((*argvp) + numtokens) = NULL;

  	return numtokens;
}
void free_makeargv(char **argv) 
{
        if (argv)
        {
                if (*argv != NULL)
                free(*argv);
                free(argv);
        }
}

u_int8_t	cm_command (cmos_status_t * stat, const char* szline, const cmos_config_t * cfg)
{
	const char * p = szline + strlen(szline) ;
	const char* q = szline;
	char szbuf[PATH_LEN] = {0};
	char szname[PATH_LEN] = {0};
	u_int32_t i = 0;
	u_int32_t len = 0;
	
	DIR * dir = 0;      
	u_int8_t run_ok = 0;
        //struct dirent* drt = 0; 
	while(q && (*q == '/'))
	{
		q++;
		i++;
	}
	if (*(q-1) == '/')q--;

	snprintf (szbuf, p - q + 1, "%s", szline);
	
	sprintf	(szname, "%s/", (*q == '/') ? cfg->plugin_dir : stat->current_dir);
	
	len = strlen (szname);
	


	for (; i < strlen(szbuf); i++)
	{
		if (isspace(szbuf[i]))
		{
			if (szname[len - 1] != '/')
			{
				szname[len] = '/';
				len++;
			}
		}else
		{
			szname[len] = szbuf[i];
			len++;
		}					
	}
	if (szname[len-1] == '/') 
	{
		szname[len-1] = '\0';
		len--;
	}
	//try to open dir
	if( (dir = opendir (szname)) != NULL )
	{
		u_int32_t index = 0;
		closedir (dir);
		memset (stat->current_dir, 0, sizeof(stat->current_dir));
		sprintf (stat->current_dir, "%s", szname);

		run_ok = 1;

		//if (szline[0] == '/')
		if (*q == '/')
		{
			memset (stat->show_path, 0, PATH_LEN);
			//index = 1;
			index = q - szline;
			stat->show_path[0] = '/';
			i = 1;
			
		}else
		{
			i = 0;
			index = strlen(stat->show_path);
			if(index > 1)
			{
				stat->show_path[index] = ' ';
				index++;
			}
		}

		for (; i < strlen(szbuf); i++)
		{
			if (isspace(szbuf[i]))
			{
				if (!isspace(stat->show_path[index - 1]))
				{
					stat->show_path[index] = ' ';
					index++;
				}
			}else
			{
				stat->show_path[index] = szbuf[i];
				index++;
			}					
		}
		return RET_OK;
	}else //open file
	{
		if (!access(szname, R_OK))
		{
			run_ok = 1;
			return cm_run_cmd (stat, szname, szline, cfg);
		}

	}
	if (!run_ok)
	{
		printf ("bad command name:%s\n", szline);
	}
	return RET_OK;
}

//u_int8_t      cm_run_cmd (cmos_status_t * stat, const char* szfile, const char* szraw)
u_int8_t      cm_run_cmd (cmos_status_t * stat, const char* szfile, const char* szraw, const cmos_config_t * cfg)
{
	//printf ("run command:\nszfile=%s\nszraw=%s\n", szfile, szraw);
	
	pid_t pid = 0;	
	

	//int32_t fd[2];
	char szstore[512] = {0};
	char szbuf[1024] = {0};
	char szname[512] = {0};
	char szuser[128] = {0};
	uid_t uid = 0;
	if (search_var (szfile, "user", szuser) <= 0)
	{
		printf ("search[%s-%s]failed\n", szfile, "user");
		return 1;
	}
	
	if( get_userid (szuser, &uid) <= 0)
	{
		printf ("get user id of '%s' failed\n", szuser);
		return 1;
	}

	if (search_var (szfile, "store", szstore) > 0)
	sprintf (szname, "%s", szstore);
	else
	sprintf (szname, "%s", szfile);

	pid = fork();	
	
	if  (pid < 0)
	{
		printf ("fork failed\n");
		return RET_OK;
	}

	if (pid == 0)
	{
		const char * p = szraw;
		char  name[128] = {0};
		char ** argv_list = NULL;
		char ** argvs = NULL;
		int32_t ntok = 0;
		int32_t i = 0;
		int32_t size = 0;
		//char* envp[]  = {"env", NULL};
		if (!getuid()&& setuid(uid))
		{
			printf ("setuid(%u) failed\n", uid);
			exit (0);
		}
		while (!isspace(*p) && *p)
		{
			p++;
		}
		dup2 (1, 2);

		sprintf (name, "%s", basename(szname));
		if (*p)
		{	
			ntok = makeargv (p, &argv_list);
			/*for (i = 0; i < ntok; i++)
			{
				printf ("token[%d]=%s\n", i, argv_list[i]);
			}*/
		}else  ntok = 0;

		if (ntok < 0 )
		{
			printf ("error parameter:[%s]\n", p);
			exit (0);
		}
		size = (ntok > 0) ? ntok : 0; 
		argvs  = (char**)malloc (sizeof(char*)*(size + 2));
		if (!argvs)
		{
			printf ("malloc failed\n");
			exit (0);
		}
		memset (argvs, 0, sizeof(char*)*(size + 2));
		argvs[0] = name;
		
		for (i = 0; i < size; i++)
		argvs [i + 1] = argv_list [i];
		
		argvs [size + 1] = NULL;

		memset (szbuf, 0, sizeof(szbuf));
		if (access (szname, X_OK))
		{
			perror("");
			exit (0);
		}
		if (execvp(szname,  argvs) < 0)
		{
			perror("Error on execv:");
		}
		//close (fd[1]);
		free(argvs);
		if (ntok >= 0)
		free_makeargv (argv_list);
		exit(0);
	}else
	{
		time_t time_last;//
		time(&time_last);// = time (0);
		//int run_time_limit = 45;
		int run_time_limit = cfg->run_time_out;
		pid_t ret_pid = 0;
		//printf ("waiting child process %d\n", pid);
		//memset (szbuf, 0, sizeof(szbuf));
		do
		{	ret_pid = waitpid (pid, NULL, WNOHANG);
			if (ret_pid <= 0)
			{
				//printf ("%d:%d\n", __LINE__, ret_pid);
				usleep (50);
			}else
			{
				break;
			}
			//wait(NULL);
		}while (time (0) - time_last <= run_time_limit);
		if (ret_pid <= 0)
		{
			kill (pid, SIGTERM);
			//printf ("%d:%d\n", __LINE__, ret_pid);
			usleep (50);
			if (waitpid (pid, NULL, WNOHANG) <= 0)
			{
				kill (pid, SIGKILL);
			}
			printf ("task with id %d run time out, just kill it\n", pid);
		}
	}	
	return RET_OK;
}

u_int8_t	cm_cmd (cmos_status_t * pstat, const char* szline, const cmos_config_t * cfg)
{
	char szdata[1024] = {0};
	char szpath[PATH_LEN] = {0};
	char szshow[PATH_LEN] = {0}; 
	char * next, *val;
	command_t pcmd;
	u_int8_t  find_dir = 0;
	u_int8_t error = 0;
	u_int8_t first = 1;
	cmd_list_t cmd;
	u_int8_t find_root = 0;
	u_int32_t len = strlen(szline);
	if (len > 1024) len = 1024;
	strncpy (szdata, szline, len);
	
	//memcpy(&cmd, pstat->cmd_list, sizeof(cmd_list_t));
	cmd.cmd_num = 0;
	strncpy (szpath, pstat->current_dir, strlen(pstat->current_dir));	
	strncpy (szshow, pstat->show_path, strlen(pstat->show_path));
	val = szdata;
	next = strsep (&val, " ,\t");
	
	load_bins (szpath, &cmd);	
	
	while (next)
	{
		struct stat st;
		int32_t i = 0;
		char * p = next;
		u_int8_t exit = 0;
		u_int8_t find = 0;
		find_dir = 0;
		if (strlen(p) == 0) goto LOOP;
	
		if (first)
		{
			if (next[0] == '/')
			{
				p = next;
				while (p&&(*p == '/'))p++;
				p--;
				find_root = 1;
				load_bins (cfg->plugin_dir, &cmd);
				memset (szpath, 0, strlen(szpath));
				memset (szshow, 0, strlen(szshow));
				if (strcmp(p, "/") == 0)
				{
					sprintf (szshow, "%s", "/");
					sprintf (szpath, "%s", cfg->plugin_dir);
					first = 0;
					//printf ("----------:%s\n", p);
					goto LOOP;
					//continue;
				}
				else
				{
					sprintf (szshow, "%s", "/");
					sprintf (szpath, "%s/%s", cfg->plugin_dir, p+1);
					p++;
				}
			}
		}

		if (!first || !find_root)
		{
			//printf ("fffffffff\n");
			strcat (szpath, "/");
			strcat (szpath, p);
			
		}
		//printf ("szpath:%s\n", szpath);	
		first = 0;	
		//dir or file exists?
		//if (stat (szpath, &st))
		if (stat (szpath, &st))
		{
			//printf ("%s\r\n", szpath);
			//perror("");
			error = 1;
			break;
		}
		//find it in list
		for (i = 0; i < cmd.cmd_num; i++)
		{
			if (strcmp(cmd.cmd_list[i].name, p) == 0)
			{
				//pcmd  = cmd.cmd_list + i;
				memcpy(&pcmd, cmd.cmd_list + i, sizeof(pcmd));
				find_dir = 1;
				find = 1;
				break;
			}
		}
		if (!find)
		{
			//printf ("--not find %s\r\n", p);
			error = 1;
			break;
		}
		//check current dir or file
		switch(st.st_mode & S_IFMT)
		{
			case S_IFDIR:
			{
				if (strlen(szshow) > 1)
				strcat (szshow, " ");
				strcat (szshow, p);
				load_bins (szpath, &cmd);
				//printf ("==[%s][%s] load %d\n", szshow, szpath, tnum);
				//printf ("--%p\n", pcmd);
				break;
			}
			case S_IFREG:
			{
				cm_run_cmd (pstat, szpath, szline, cfg);
				find_dir = 0;
				find_root = 0;
				exit = 1;
				break;
			}
			default:
			{
				printf ("bad file format\r\n");
				error = 1;
				break;
			}
		}
		if (error) break;
		if (exit) break;
		//load no bins
		LOOP:
		next = strsep (&val, " ,\t");	
	}
	if (error)
	{
		//printf ("bad command name:"LIGHT_RED"%s"NONE"\n", szline);
		printf ("bad command name:%s\n", szline);
	}else
	{
		if ((find_dir && (pcmd.type == TYPE_PKG))||find_root)
		{
			memset (pstat->current_dir, 0, sizeof(pstat->current_dir));
			strncpy (pstat->current_dir, szpath, strlen(szpath));
			memset (pstat->show_path, 0, sizeof(pstat->show_path));
			strncpy (pstat->show_path, szshow, strlen(szshow));
		}
	
	}
	return RET_OK;
}
//run command with the help of red black tree
u_int8_t	cm_rbcmd (cmos_status_t * pstat, const char* szline, const cmos_config_t * cfg)
{
	char szdata[1024] = {0};
	char szpath[PATH_LEN] = {0};
	char szshow[PATH_LEN] = {0}; 
	char * next, *val;
	
	u_int32_t input_type = 0;
	u_int8_t  find_dir = 0;
	u_int8_t error = 0;
	u_int8_t first = 1;
	u_int8_t find_root = 0;
	u_int32_t len = strlen(szline);
	if (len > 1024) len = 1024;
	strncpy (szdata, szline, len);
	strncpy (szpath, pstat->current_dir, strlen(pstat->current_dir));	
	strncpy (szshow, pstat->show_path, strlen(pstat->show_path));
	val = szdata;
	next = strsep (&val, " ,\t");
	
	
	while (next)
	{
		struct stat st;
		char * p = next;
		u_int8_t exit = 0;
		u_int8_t find = 0;
		find_dir = 0;
		char name[256] = {0};
		char store[PATH_LEN] = {0};
		u_int8_t  store_ok = 0;
		if (strlen(p) == 0) goto LOOP;
	
		if (first)
		{
			if (next[0] == '/')
			{
				p = next;
				while (p&&(*p == '/'))p++;
				p--;
				find_root = 1;
				memset (szpath, 0, strlen(szpath));
				memset (szshow, 0, strlen(szshow));
				if (strcmp(p, "/") == 0)
				{
					sprintf (szshow, "%s", "/");
					sprintf (szpath, "%s", cfg->plugin_dir);
					first = 0;
					//printf ("----------:%s\n", p);
					goto LOOP;
					//continue;
				}
				else
				{
					sprintf (szshow, "%s", "/");
					sprintf (szpath, "%s/%s", cfg->plugin_dir, p+1);
					p++;
				}
			}
		}//end of if first

		if (!first || !find_root)
		{
			//printf ("fffffffff\n");
			strcat (szpath, "/");
			strcat (szpath, p);
			
		}
		//printf ("szpath:%s\n", szpath);	
		first = 0;	
		//dir or file exists?
		//if (stat (szpath, &st))
		if (stat (szpath, &st))
		{
			//added by jigang.djg@2011-11-01 --start
			if (search_var (szpath, "store", store) <= 0)
			{	
				//added by jigang.djg@2011-11-01 --finish
				error = 1;
				break;
			}else store_ok = 1;
		}
		
		if (search_var(szpath, "name", name) > 0)
		{
			char type[10] = {0};
			char enable[10] = {0};
			int8_t ntype, non;
			ntype = search_var (szpath, "type", type);
			non   = search_var (szpath, "enable", enable);
			if ((ntype >0) && (non > 0)) 
			{
				input_type = (strcmp(type, "dir")== 0) ? TYPE_PKG : TYPE_CMD;
				find_dir = 1;
				find = 1;
			}
		}else
		{
			printf ("can not find 'name' of '%s'\n", szpath);
		}

		if (!find)
		{
			//printf ("--not find %s\r\n", p);
			error = 1;
			break;
		}
		//added by jigang.djg@2011-11-01 --start
		if (store_ok)
		{

				if (cfg->op_mask == OP_PLUGIN_CMD)
				cm_run_cmd (pstat, szpath, szline + (next - szdata), cfg);
				else
				if (cfg->op_mask == OP_INNER_CMD)
				cm_cat_cmd (pstat, szpath, szline + (next - szdata));
				find_dir = 0;
				find_root = 0;
				exit = 1;
				break;
		}
		//added by jigang.djg@2011-11-01 --finish
		//check current dir or file
		switch(st.st_mode & S_IFMT)
		{
			case S_IFDIR:
			{
				if (strlen(szshow) > 1)
				strcat (szshow, " ");
				strcat (szshow, p);
				break;
			}
			case S_IFREG:
			{
				if (cfg->op_mask == OP_PLUGIN_CMD)
				cm_run_cmd (pstat, szpath, szline + (next - szdata), cfg);
				else
				if (cfg->op_mask == OP_INNER_CMD)
				cm_cat_cmd (pstat, szpath, szline + (next - szdata));
				find_dir = 0;
				find_root = 0;
				exit = 1;
				break;
			}
			default:
			{
				printf ("bad file format\r\n");
				error = 1;
				break;
			}
		}
		if (error) break;
		if (exit) break;
		//load no bins
		LOOP:
		next = strsep (&val, " ,\t");	
	}
	if (error)
	{
		char* flag = szpath + strlen(szpath) - 1;
		if (*flag == '?')
		{
			*flag = '\0';
			if (cfg->op_mask & OP_PLUGIN_CMD)
			cm_rbshow (szpath, cfg);
		}else
		{
			printf ("bad command name:%s\n", szline);
		}
	}else
	{
		if ((find_dir && (input_type == TYPE_PKG))||find_root)
		{
			memset (pstat->current_dir, 0, sizeof(pstat->current_dir));
			strncpy (pstat->current_dir, szpath, strlen(szpath));
			memset (pstat->show_path, 0, sizeof(pstat->show_path));
			strncpy (pstat->show_path, szshow, strlen(szshow));
		}
	
	}
	return RET_OK;
}
u_int8_t      cm_cat_cmd (cmos_status_t * stat, const char* szfile, const char* szraw)
{
	//printf ("szfile=%s,data=%s\n", szfile, szraw);
	char szline[4096] = {0};
	char tpath [256] = {0};
	FILE * fp = NULL;
	if (search_var (szfile, "store", tpath) > 0)
	fp = fopen (tpath, "r");
	else
	fp = fopen (szfile, "r");
	if (fp)
	{
		while (fgets (szline, 4096, fp))
		{
			printf ("%s", szline);
			memset (szline, 0, 4096);
		}
		fclose (fp);
	}
	return RET_OK;
}
