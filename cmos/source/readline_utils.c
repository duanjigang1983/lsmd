#include "cmos.h"

#ifdef _USE_READLINE_
typedef struct _complete_node
{
	char * address;
	u_int16_t length;
	u_int8_t valid;
	struct _complete_node * children;
}complete_node_t;
typedef struct
{
	complete_node_t list[MAX_PLUGINS];
	u_int16_t num;
}complete_t;
complete_t 	g_cm;
extern char * command_generator (const char* text, int state);
extern char * command_generator_test (const char* text, int state);
char * g_current_path = 0;
char * g_plugin_path = 0;
//int32_t  g_pos = 0;
int32_t cm_sort( const void *a, const void *b )
{
	complete_node_t * a1, *a2;
	a1 = (complete_node_t*)a;
	a2 = (complete_node_t*)b;
	return strcmp(a1->address, a2->address);
}
char ** cm_completion (const char* text, int start, int end)
{
	char **matches;
  	matches = (char **)NULL;
	matches = rl_completion_matches (text, command_generator);
  	return (matches);
}

void init_readline(cmos_status_t * stat, cmos_config_t * cfg)
{
	// Allow conditional parsing of the ~/.inputrc file.
  	u_int16_t i = 0;
	rl_readline_name = "";
	g_current_path = stat->current_dir;
	g_plugin_path = cfg->plugin_dir;
  	// Tell the completer that we want a crack first. 
  	rl_attempted_completion_function = cm_completion;
	g_cm.num = 0;
	for (i = 0; i < MAX_PLUGINS; i++)
	{
		g_cm.list[i].valid = 0;
		g_cm.list[i].address = 0;
		g_cm.list[i].length = 0;
		g_cm.list[i].children = 0;
	}
}

char * command_generator_test (const char* text, int state)
{
	static int num = 5;
	char szbuf[128] = {0};
	if (num-- > 0)
	{
		sprintf (szbuf, "test_%d", num);
		return strdup(szbuf);
	}else
	{
		num = 5;
		return NULL;
	}
}
char * command_generator (const char* text, int state)
{
	
	char szbuf[1024] = {0};
	char * p = rl_line_buffer;
	char ret[1024] = {0};
	u_int8_t find_root = 0;
	char * index = 0;
	static complete_node_t * pos = 0;
	static u_int16_t	pos_index = MAX_PLUGINS;
	static u_int8_t 	first_time = 1;
	u_int8_t input_null = 0;
	
	if (!state)
	{
		pos = NULL;
		pos_index = MAX_PLUGINS;
		first_time = 1;
	}
	
	if (!g_current_path || !g_plugin_path)
	{
		printf (""RED"current_path or plugin_path null"NONE"\n");
		return (char*)NULL;
	}
	//printf (""RED"|%s|"NONE"\n", rl_line_buffer);
	strcpy (ret, rl_line_buffer);
	p = stripwhite (ret);
	if (!*p)
	{
		//printf (""RED" strip null "NONE"\n");
		input_null = 1;
		 //return (char*) NULL;
		 //return strdup("help");
	}

	if (!input_null)
	{
		while(p && (*p=='/'))
		{
			find_root = 1;
			p++;
		}
	
		if (!p && !find_root)
		{
			 printf (""RED"!p && !find_root "NONE"\n");
		 	return (char*)NULL;
		}
		while(isspace(*p)&& p )p++;
		if (!p && !find_root)
		{
			 printf (""RED"!p && !find_root again"NONE"\n");
		 	return (char*)NULL;
		}
		if (!find_root)
		{
			strcat (szbuf, g_current_path);
		}else
		{
			strcat (szbuf, g_plugin_path);	
		}
		if (p)
		strcat (szbuf, "/");
		index = szbuf + strlen(szbuf)-1;
		while (*p)
		{
			if (isspace(*p))
			{
				if (*index != '/')
				{	
					index++;
					*index = '/';
				}
				p++;
				continue;	 
			}
			index++;
			*index = *p;
			p++;
			continue;
		}
	}else
	{
		strcat (szbuf, g_current_path);
	}
	//printf ("\n==%s\n", szbuf);
	if ((pos != NULL) && (pos_index < MAX_PLUGINS))
	{
		char szline[1024] = {0};
		pos = pos->children;
		if (pos == NULL)
		{
			return (char*)NULL;
		}
		sprintf (szline, "%s", pos->address);
		return strdup(basename(szline));	
	}else
	{
		u_int16_t len = strlen(szbuf);
		u_int16_t i = (pos_index != MAX_PLUGINS) ? (pos_index) : 0;
		//first_time = 1;
		for (; i < g_cm.num; i++)
		{
			if (strncmp(szbuf+strlen(g_plugin_path), g_cm.list[i].address+strlen(g_plugin_path),
				 len-strlen(g_plugin_path)) == 0)
			{
				break;
			}
		}
		if (i >= g_cm.num)
		{
			return (char*)NULL;
		}
		pos_index = i;

		if (len == g_cm.list[i].length)
		{
			pos = g_cm.list[i].children;
			if (pos)
			{
				char szline[1024] = {0};
                		sprintf (szline, "%s", pos->address);
                		return strdup(basename(szline));
			}else 
			{
				return (char*)NULL;
			}
		}else
		{
			char szline[1024] = {0};
			sprintf (szline, "%s", szbuf);
			char * q = 
			strstr(g_cm.list[i].address + strlen( g_plugin_path ), 
				basename(szbuf + strlen(g_plugin_path)));
			char * p = g_cm.list[i].address + len;
			q = p;
			while(*q != '/')q--;
			q++;
			while(*p != '/' && *p)p++;
			char aaa[256]  = {0};
			strncpy(aaa, q, p - q);
			char * ret = strdup (aaa);
			pos_index = i;
			pos = NULL;
			pos_index++;
			return ret;
		}

		
	}
	return NULL;
}
u_int8_t	add_completion	(const char* path, u_int8_t valid)
{
	if (!valid) return 0;
	u_int16_t len = strlen (path);
	u_int16_t i = 0;
	char szline[1024] = {0};
	char* p = szline;
	sprintf (szline, "%s", path);
	p = dirname (szline);
	if (!p)
	{
		printf ("%s-%d:dirname(%s)failed\n", __FILE__, __LINE__, szline);
		return 0;
	}
	if (g_cm.num >= MAX_PLUGINS)
	{
		printf ("too much plugin to add <%u>\n", g_cm.num);
		return 0;
	}	
	
	g_cm.list [g_cm.num].address = (char*)malloc (len+1);
	if (!g_cm.list [g_cm.num].address)
	{
		printf ("%s-%d:malloc(%u) failed\n", __FILE__, __LINE__, len+1);
		return 0;
	}	
	memset (g_cm.list [g_cm.num].address, 0, len + 1);
	strncpy (g_cm.list [g_cm.num].address, path, len);
	g_cm.list [g_cm.num].address[len] = '\0';
	g_cm.list [g_cm.num].length = len ;
	//find its parent
	
	for (i = 0; i < g_cm.num; i++)
	{
		
		if (strcmp(p, g_cm.list[i].address) == 0)
		{
			complete_node_t * q = (complete_node_t*)malloc(sizeof(complete_node_t));
			if (!q)
			{
				printf ("%s-%d:malloc(complete_node_t)failed\n", __FILE__, __LINE__);
				return 0;
			}			
			memcpy (q, g_cm.list + g_cm.num, sizeof(complete_node_t));
		
			q->children = NULL;
		
			if (!g_cm.list[i].children)
			{
				g_cm.list[i].children = q;	
			}else
			{
				q->children = g_cm.list[i].children;
				g_cm.list[i].children = q;
			}
			//printf ("adding son '%s-%s' at list %u\n", path, p, i);
			break;
		}
	}
	g_cm.num++;
	//printf ("adding completion:"RED"%s"NONE"\n", path);
	return 1;
}
void		sort_completion (void)
{
	qsort (g_cm.list, g_cm.num, sizeof(g_cm.list[0]), cm_sort);
	
#if 0
	u_int16_t i = 0;
	for (i = 0; i < g_cm.num; i++)
	{
		printf ("["GREEN"%03u"NONE"-"YELLOW"%s"NONE"]", 
		g_cm.list[i].length, g_cm.list[i].address);
		if (g_cm.list[i].children)
		{
			complete_node_t * p = g_cm.list[i].children;
			while (p)
			{
				printf (" %s", p->address);
				p = p->children;
			}			
		}
		printf ("\n");
	}
#endif
}
#endif
