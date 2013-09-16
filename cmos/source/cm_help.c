#include "cmos.h"
#include "IniHelper.h"

u_int8_t help_upper (const char*data,  cmos_status_t * stat,  cmos_config_t * cfg);
u_int8_t help_show (const char*data,  cmos_status_t * stat,   cmos_config_t * cfg);
u_int8_t help_pwd (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg);
u_int8_t help_help (const char*data,  cmos_status_t * stat,   cmos_config_t * cfg);
u_int8_t help_quit (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg);
u_int8_t help_cat (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg);
u_int8_t help_nothing (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg);
extern u_int8_t help_set (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg);
//inner command of cmos
help_cmd_t help_list [] = {

	{".", 		help_nothing, 	"goto upper coms\n", 0},
	{"..", 		help_upper, 	"goto upper coms\n", 1},
	{"help",	help_help , 	"print this message\n", 1},
	{"quit",	help_quit , 	"quit cmos login\n", 1},
	{"pwd", 	help_pwd, 	"view current position\n", 1},
	{"cat", 	help_cat, 	"view content of a command\n", 1},
	{"set", 	help_set, 	"set or view value of variables in cmos\n", 1},
	{"?", 		help_show, 	"list commands of this package\n", 1}
};
//----help function --- start
u_int8_t help_upper (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	return cm_upper (stat, cfg);
}
u_int8_t help_show (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	return cm_rbshow(stat->current_dir, cfg);	
}

u_int8_t help_pwd (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{		
	printf ("%s\n", stat->show_path);
	return RET_OK;
}
u_int8_t help_help (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	print_help ();
	return RET_OK;
}
u_int8_t help_nothing (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	return RET_OK;
}
u_int8_t help_quit (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	cfg->quit_flag = 1;
	return RET_EXIT;
}

u_int8_t help_cat (const char*data,  cmos_status_t * stat,    cmos_config_t * cfg)
{
	//printf ("cat:%s\n", data);
	cfg->op_mask = OP_INNER_CMD;
	return cm_rbcmd (stat, data, cfg);
}

//----help function --- finishe

int16_t   	handle_innercmd	(const char* szline,	cmos_status_t * stat,	 cmos_config_t * cfg)
{
	u_int16_t i = 0;
	char szbuf [1024] = {0};
	char * p = szbuf;
	char q = 0;
	sprintf (szbuf, "%s", szline);
	while(*p && !isspace(*p) )p++;
	//if (!*p) return -1;
	q = *p;
	*p = '\0';
	for ( i = 0; i < sizeof(help_list)/sizeof(help_cmd_t); i++)
	{
		if (strcmp (help_list[i].name, szbuf) == 0)
		{
			*p = q;
			if (help_list[i].func)
			{
				
				return help_list[i].func (szbuf + strlen(help_list[i].name), stat, cfg);
			}
			else
			{
					return RET_OK;
			}
		}
	}
	return -1;	
}
void print_help(void)
{
	u_int16_t i = 0;
	
	for (i = 0; i < sizeof(help_list)/sizeof(help_cmd_t); i++)
	{
		if (help_list[i].printable)
		printf ("%-10s\t%s", help_list[i].name, help_list[i].desc);
	}
	/*
	printf ("%-10s\tgoto upper cmos\n", "..");
	printf ("%-10s\tprint this message\n", "help");
	printf ("%-10s\tview current position\n", "pwd");
	printf ("%-10s\tlist commands in this step\n", "?");
	*/
}

u_int8_t 	cm_upper (cmos_status_t *stat, const cmos_config_t * cfg)
{
	char * p = stat->current_dir + strlen(stat->current_dir);
	char * q = p;
	//already top dir
	if (strlen(stat->current_dir) == strlen(cfg->plugin_dir))
	{
		return RET_OK;
	}
	//while ((*p != '/') && (p != stat->current_dir + strlen(DATA_DIR)))
	while (*p != '/')
	{
		p--;
	}
	/*
	if (p == stat->current_dir)
	{
		return RET_BUG;
	}
	*/
	while (q != p)
	{
		*q = '\0';
		 q--;
	}
	*p = '\0';

	//back show path
	p = stat->show_path + strlen(stat->show_path);
	q = p;
	
	while (!isspace(*p) && (*p != '/'))p--;	
	while(isspace(*p))p--;
	
	/*if (*p == '/')
	{
		 return RET_OK;	
	}*/
	while (q != p)
	{
		*q = '\0';
		q--;
	}
	return RET_OK;
}


int32_t	load_bins (const char* path, cmd_list_t * cmd_list)
{
	char szfile[PATH_LEN] = {0};
	section sc;
	void * ini = NULL;
	struct stat st;

	if (stat (path, &st))
	{
		return -1;
	}

	sprintf (szfile, "%s/%s", path, CTL_FILE);		
	cmd_list->cmd_num = 0;

	if(NULL == (ini = init_ini(szfile)))
	{
		return 0;
	}
	
	
	while (read_section(ini, &sc))
	{
		u_int32_t index = 0;
		struct stat st;
		u_int8_t ok = 0;

		char fname[800] = {0};

		command_t cmd;
		memset (&cmd, 0, sizeof(command_t));
		
		for (index = 0; index < sc.entry_number; index++)
		{
			//read name
			if (strncasecmp(sc.entry_list[index].var_name, "name", 4) == 0)
                        {
                                sprintf (cmd.name, "%s", sc.entry_list[index].var_value);
                                continue;
                        }
			//read comment information
			if (strncasecmp(sc.entry_list[index].var_name, "desc", 4) == 0)
                        {
                                sprintf (cmd.desc, "%s", sc.entry_list[index].var_value);
                                continue;
                        }
			//read on flag
			if (strncasecmp(sc.entry_list[index].var_name, "enable", 6) == 0)
                        {
                                cmd.enable = atoi (sc.entry_list[index].var_value);
                                continue;
                        }
		}
		//invalid node
		if (!strlen(cmd.name) || ! strlen(cmd.desc)) continue;
		//construct a file name or a dir name
		sprintf (fname, "%s/%s", path, cmd.name);

		//try to stat
		if (stat (fname, &st)) continue;
		//check status
		switch (st.st_mode & S_IFMT)
		{	
			//dir package
			case S_IFDIR:
					cmd.type = TYPE_PKG;
					ok = 1;
					break;
			//common file command
			case S_IFREG:
					cmd.type = TYPE_CMD;
					ok = 1;
					break;
			default:break;
		}
		//check
		if (!ok) continue;
		//copy
		memcpy (cmd_list->cmd_list + cmd_list->cmd_num, &cmd, sizeof(command_t));
		//check too much command
		if (++cmd_list->cmd_num >= MAX_CMD_EACH_DIR)
		{
			break;
		}
	}
	close_ini(ini);	
	return cmd_list->cmd_num;
}
