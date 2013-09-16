#include "cmos.h"
#include <ctype.h>

int16_t getch(void)
{
        struct termios tm, tm_old;
        int32_t fd = STDIN_FILENO, c;

        if(tcgetattr(fd, &tm) < 0)
        {
	        return -1;
	}
        tm_old = tm;

        cfmakeraw(&tm);

        if(tcsetattr(fd, TCSANOW, &tm) < 0)
         {
		return -1;
	}
        c = fgetc(stdin);

        if(tcsetattr(fd, TCSANOW, &tm_old) < 0)
         {
		       return -1;
	}
        return c;
}

u_int8_t	init_status (cmos_status_t *stat, const cmos_config_t * cfg)
{
	DIR * dir = NULL;
	char szfile[512] = {0};
	memset (stat, 0, sizeof(cmos_status_t));
	sprintf (stat->current_dir, "%s", cfg->plugin_dir);
	if( (dir = opendir (stat->current_dir)) == NULL )
	{
		fprintf (stderr, "open dir '%s' failed\n", stat->current_dir);
		return RET_ACCESS_FAILED;
	}
	closedir (dir);

	sprintf (szfile, "%s/%s", cfg->plugin_dir, CTL_FILE);
	if (access (szfile, R_OK))
	{
		fprintf (stderr, "can not find file:'%s'\n", szfile);
		return RET_ACCESS_FAILED;
	}
	sprintf (stat->show_path, "/");

	//malloc memory for cmd_list;
	if (stat->cmd_list)
	{
		free(stat->cmd_list);
		stat->cmd_list = 0;
	}
	if ((stat->cmd_list = (cmd_list_t*)malloc(sizeof(cmd_list_t)))== NULL)
	{
		fprintf (stderr, "malloc for cmd_list failed\n");
		return RET_MALLOC_FAILED;
	}
	memset (stat->cmd_list, 0, sizeof(cmd_list_t));
	stat->cmd_list->cmd_num = 0;
	return RET_OK;
}

/*
read one line from stdin
*/
//u_int8_t mode_normal = 0;

int32_t	read_line	(char* szbuf, u_int32_t length, const cmos_config_t * cfg)
{
	alarm (cfg->input_time_out);
	do
	{
		char * ptr = NULL;
		memset (szbuf, 0, length);
		#ifdef _USE_READLINE_
		char* line = readline("");	
		char * s = 0;
		if (!line)
		{
			break;
		}
		s = stripwhite (line);
		if (*s)
		{
			add_history (s);
			strcpy (szbuf, s);
		}
		free (line);
		#else
		//sprintf (szbuf, "%s", fgets(szbuf, length, stdin));
		ptr = fgets (szbuf, length, stdin);
		if (!ptr)
		{
			szbuf[0] = '\0';	
		}
		#endif
	}while (0);
	alarm (0);
	return strlen(szbuf);
}
char * stripwhite (char * string)
{
  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;

  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}

int32_t get_userid (const char* name, uid_t * uid)
{
	char szline[1024] = {0};
	u_int8_t find = 0;
	FILE * fp = fopen ("/etc/passwd", "r");
	if (!fp) return -1;
	
	while (fgets (szline ,1024, fp))
	{
		char fname[128] = {0},szuid[10] = {0};
		char * p1, * p2, *p3;

		p1 = strchr (szline, ':');
		if (!p1)continue;
		p2 = strchr (p1+1, ':');
		if (!p2)continue;
		p3 = strchr (p2+1, ':');
		if (!p3) continue;
		strncpy (fname, szline, p1-szline);
		if (strcmp(name, fname)) continue;
		strncpy(szuid, p2 + 1, p3-p2-1);
		//printf ("===%s-%d\n", uid, strlen(uid));
		*uid = atoi(szuid);
		find  = 1;
		break;	
	}
	return find ? 1:-1;
}

