#include "cmos.h"
#include "rbtree.h"
#include "IniHelper.h"

//tree of plugin
struct rb_root	bin_tree;

//define element of a plugin
typedef struct _elem_node_t
{
	struct rb_node node;
	char * var_name;//var name
	char * var_value; //var value
}elem_node_t;

//define a plugin
typedef struct _path_node_t
{
	struct 	rb_node node;
	char*	path; //path name
	struct rb_root elem_tree;
	elem_node_t elem_node; //element tree
}path_node_t;
path_node_t root_path_node;

//used to parse all dirs
typedef  struct _dir_node
{
	char * name;
	struct _dir_node * next;
	u_int8_t valid;
}dir_node;

dir_node * g_dir_list = 0;
int32_t	   g_dir_num = 0;
int8_t	   g_rbtree_init = 0;

static int32_t	load_dir_list (const char* path, const cmos_config_t * cfg);
static int32_t	load_dir_bins(cmos_status_t * stat, const cmos_config_t * cfg);
static int32_t get_path_node_var (path_node_t * pn, const char* name, char* value);
int32_t	load_plugins (const char* path, cmos_status_t * stat, const cmos_config_t * cfg )
{
	int32_t nret = load_dir_list(path, cfg);
	switch (nret)
	{
		case RET_BUG: printf ("%s-%d:there must be a bug\n", __FILE__, __LINE__); return -1;
		case RET_MALLOC_FAILED: printf("%s-%d:malloc failed\n", __FILE__, __LINE__); return -1;
		default:break;
	}
	load_dir_bins (stat, cfg);
	return RET_OK;
}
//load all plugins in dir "path"
int32_t	load_dir_list (const char* path, const cmos_config_t * cfg)
{
	//dir_node list
	dir_node * pn = 0;
	u_int32_t  ret = 0;
	dir_node * current_pointer = 0;
	dir_node * tail = 0;
	//dir number
	u_int32_t num = 0;	
	if (!path) return RET_BUG;

	pn = (dir_node*) malloc(sizeof(dir_node));
	if (!pn)
	{
		printf ("%s-%d:malloc failed\n", __FILE__, __LINE__);
		return RET_MALLOC_FAILED;
	}

	memset (pn, 0, sizeof(dir_node));
	pn->name = (char*) malloc (strlen(path)+1);
	if (!pn->name)
	{
		printf ("%s-%d:malloc failed\n", __FILE__, __LINE__);
		free (pn);
		return RET_MALLOC_FAILED;

	}
	memset (pn->name, 0, strlen(path)+1);
	strncpy (pn->name, path, strlen(path));
	pn->next = 0;
	//pn->valid = 0;
	pn->valid = 1; //modified by duanjigang@2011-09-28 11:52
	tail = pn;
	num = 1;

	//start to parse
	while (num > 0)
	{
		DIR * dir = 0;      
        	struct dirent* drt = 0; 
		current_pointer = pn;
	
		//try to open dir
		if( (dir = opendir (current_pointer->name))== NULL )
		{
			printf ("%s-%d:open dir '%s' failed\n", __FILE__, __LINE__, current_pointer->name);
			goto LOOP;
		}
		//printf ("open:%s\n", current_pointer->name);
		//loop this dir
		while ((drt = readdir(dir)) != NULL)
        	{
			dir_node  * nd = 0;
			u_int32_t len = 0;

			if ((strcmp(drt->d_name, ".") == 0) || (strcmp(drt->d_name, "..") == 0))
                	{
                        	continue;
                	}
			//printf ("%s/%s\n", current_pointer->name,drt->d_name);	
			if (drt->d_type != DT_DIR)
			{
				//printf ("not dir "RED"%s"NONE"\n", drt->d_name);
				continue;
			}
			//add this dir into our dir queue
			nd = (dir_node*)malloc (sizeof(dir_node));
			if (!nd)
			{
				printf ("%s-%d:malloc failed\n", __FILE__, __LINE__);
				continue;
			}
			memset (nd, 0, sizeof(dir_node));
			len = strlen (current_pointer->name) + strlen (drt->d_name) + 2;
			nd->name = (char*) malloc (len);
			if (!nd->name)
			{
				printf ("%s-%d:malloc failed\n", __FILE__, __LINE__);
				free (nd);
				continue;

			}
			memset (nd->name, 0, len);
			sprintf (nd->name, "%s/%s", current_pointer->name, drt->d_name);	
			//printf ("pushing "GREEN"%s"NONE"\n", nd->name);
			nd->next = 0;
			nd->valid = 1;
			tail->next = nd;
			tail = nd;
			num++;
		}
		closedir (dir);
	LOOP:
		num--;
		pn = pn->next;
		
		//printf ("poping:"YELLOW"%s"NONE"\n", current_pointer->name);
		if (!g_dir_list)
		{
			g_dir_list = current_pointer;
			g_dir_list->next = 0;
		}
		else
		{
			current_pointer->next  = g_dir_list;
			g_dir_list = current_pointer;
		}
		ret++;
	}
	
	pn = g_dir_list;
	//reverse this list
	current_pointer = pn->next;

	while (current_pointer)
	{
		dir_node * p = current_pointer->next;
		current_pointer->next = pn;
		pn = current_pointer;
		current_pointer = p;
	}
	g_dir_list = pn;
		
	pn = g_dir_list;
	for (num = 0; num < ret; num++)
	{
		//printf (""YELLOW"%s"NONE"\n", pn->name);
		pn = pn->next;
	}
	g_dir_num = ret;
	return RET_OK;
}
int32_t	load_dir_bins (cmos_status_t * pstat, const cmos_config_t * cfg)
{
	
	int32_t i  = 0;
	if (!g_dir_list) return 0;
	g_dir_list->valid = 1;
	
	dir_node * pn = g_dir_list;

	//added by duanjigang@2011-09-22 10:20 --start
	static u_int8_t add_root_dir = 0;
	
	//adding root dir	
	if (!add_root_dir)
	{
		insert_var (cfg->plugin_dir, "name", cfg->plugin_dir);
		insert_var (cfg->plugin_dir, "type", "dir");
		insert_var (cfg->plugin_dir, "desc", "root dir of cmos");
		insert_var (cfg->plugin_dir, "enable", "1");
		//insert_var (cfg->plugin_dir, "maintanier", "tbsa");
		//add_completion (cfg->plugin_dir, 1);
	}
	//added by duanjigang@2011-09-22 10:20 --end

	//adding plugins in each directory
	for (i = 0; i < g_dir_num; i++)
	{
		char szfile[512] = {0};
		char tdesc[128] = {0};
		char szuser[128] = {0};//added by jigang.djg@2011-11-01
		char szmaint[128] = {0};//added by jigang.djg@2012-01-09
		int8_t tenable = -1;
		//char path[256] = {0};
		section sc;
		void * ini = NULL;
		if (!pn->valid) goto LOOP;
		//construct a file name
		sprintf (szfile, "%s/%s", pn->name, CTL_FILE);
		//printf ("access:"RED"%s"NONE"\n", szfile);
		if (access (szfile, R_OK))
		{
			fprintf (stderr, "can not find file '%s'\n", szfile);
			goto LOOP;
		}else
		{
			//printf ("find file"GREEN" %s"NONE"\n", szfile);
		}	
		//read this file
		if(NULL == (ini = init_ini(szfile)))
		{
			fprintf (stderr, "%s-%d:open file '%s' failed\n", __FILE__, __LINE__, szfile);
			goto LOOP;
		}
		sprintf (tdesc, "%s", read_string(ini, "package", "desc", "error"));
		if (strstr(tdesc, "error"))
		{
			fprintf (stderr, "%s-%d:get package-desc failed in file '%s'\n", __FILE__, __LINE__, szfile);
			goto LOOP;
		}
		//added by jigang.djg@2011-11-01 --start
		sprintf (szuser, "%s", read_string(ini, "package", "user", "root"));	
		if (strcmp(szuser, "self") == 0)
		{
			char * next = pn->name, *pre = pn->name, *last = pn->name;
			while ((pre = strchr(next, '/')))
			{
				last = next;
				next = pre + 1;
			}
			next = strchr (last, '/');
			if (next) last = next + 1;		
			memset (szuser, 0, sizeof(szuser));
			sprintf (szuser, "%s", last);
			//printf ("self:%s\n", last);	
		}
		//added by jigang.djg@2011-11-01 --finish
		
		sprintf (szmaint, "%s", read_string(ini, "package", "maintanier", "tbsa"));	
		
		tenable = read_int (ini, "package", "enable", 0);
		
		if (tenable <=0 )
		{
			goto LOOP;
		}
		
		insert_var (pn->name, "name", "package");
		insert_var (pn->name, "type", "dir");
		insert_var (pn->name, "desc", tdesc);
		insert_var (pn->name, "enable", "1");
		insert_var (pn->name, "maintanier", szmaint);
		
		#ifdef _USE_READLINE_
		add_completion (pn->name, 1);
		#endif

		while (read_section(ini, &sc))
		{
			u_int32_t index = 0;
			char name[256] = {0};
			char szstore[256] = {0};
			char szuser1[128] = {0};
			char szmaint1[128] = {0};
			//struct stat st;

                        sprintf (name, "%s/%s", pn->name, sc.title);
			//because of link, so stat checking is not needed
			
			/*
			if (stat (name, &st)) continue;
			//check file or dir
			if((st.st_mode & S_IFMT) != S_IFREG) continue;
			*/
			
			//insert plugin name
			insert_var (name, "name", sc.title);
			//insert plugin type
			insert_var (name, "type", "file");
			
			//added by jigang.djg@2011-11-01 for szuser
			//insert_var (name, "user", szuser);
			strcpy (szuser1, szuser);
			strcpy (szmaint1, szmaint);
			//added by jigang.djg@2011-11-01 for cmdpath
			sprintf (szstore, "%s", name);
			for (index = 0; index < sc.entry_number; index++)
			{
				
				#ifdef _USE_RAEDLINE_
				if (strcmp("enable", sc.entry_list[index].var_name) == 0)
				{	
					add_completion (name, atoi(sc.entry_list[index].var_value));
				}
				#endif
				///added by jigang.djg@2012-01-09 --start
				if (strcmp (sc.entry_list[index].var_name, "user") == 0)
				{
					memset (szuser1, 0, sizeof(szuser1));
					sprintf (szuser1, "%s", sc.entry_list[index].var_value);
					continue;
				}

				if (strcmp (sc.entry_list[index].var_name, "maintanier") == 0)
				{
					memset (szmaint1, 0, sizeof(szmaint1));
					sprintf (szmaint1, "%s", sc.entry_list[index].var_value);
					continue;
				}

				///added by jigang.djg@2012-01-09 --finish

				//added by jigang.djg@2011-11-01 for cmdpath
				if (strcmp (sc.entry_list[index].var_name, "cmdpath") == 0)
				{
					memset (szstore, 0, sizeof(szstore));
					sprintf (szstore, "%s", sc.entry_list[index].var_value);
					continue;
				}else
				//added by jigang.djg@2011-11-01 finish
				insert_var (name, sc.entry_list[index].var_name, sc.entry_list[index].var_value);
			}//end of for
			
			insert_var (name, "user", szuser1);
			insert_var (name, "maintanier", szmaint1);
			//added by jigang.djg@2011-11-01 start
			insert_var (name, "store", szstore);
			//added by jigang.djg@2011-11-01 finish
		}//end of while read section
		

		LOOP:
		pn = pn->next;
		if (ini)
		{
			close_ini (ini);
		}
	} //end of for each dir
	return 1;
}
/*
void 	active_dir (const char* szpath)
{
	dir_node * pn = g_dir_list;
	int32_t num = 0;
	for (num = 0; num < g_dir_num; num++)
	{
		if (strcmp (pn->name, szpath) == 0)
		{
			pn->valid = 1;
			break;
		}
		pn = pn->next;
	}
}
*/
//insert new data to rbtree

int32_t insert_var (const char* path, const char* name, const char* value)
{
	struct rb_node ** p = NULL;
	path_node_t  * pn = NULL, *pnode = NULL;
	elem_node_t  * pe = NULL, *enode = NULL;
	struct rb_node * parent = NULL;
	u_int8_t find = 0;
	/*
	if (!g_rbtree_init)
	{
		g_rbtree_init = 1;
		memset (&root_path_node, 0, sizeof(path_node_t));	         	
		root_path_node.path = malloc(10);
		memset (root_path_node.path, 0, 10);
		sprintf (root_path_node.path, "%s", "000");
		rb_link_node (&root_path_node.node, NULL, &bin_tree.rb_node);
		rb_insert_color (&root_path_node.node, &bin_tree);
		//return 1;
	}
	*/
		
	if (!g_rbtree_init)
	{
		g_rbtree_init = 1;
		memset (&root_path_node, 0, sizeof(path_node_t));	         	
		root_path_node.path = malloc(strlen(path)+1);
		memset (root_path_node.path, 0, strlen(path)+1);
		sprintf (root_path_node.path, "%s", path);
		rb_link_node (&root_path_node.node, NULL, &bin_tree.rb_node);
		rb_insert_color (&root_path_node.node, &bin_tree);
		find = 1;
		pn = &root_path_node;
		//return 1;
	}
	else
	{
		p = &bin_tree.rb_node;
		//try
		while (*p)
		{
			int8_t res = 0;
			parent = *p;
			pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);
		
			res = strcmp (path, pnode->path);
			//compare
			if (res < 0)
			{
				//printf ("<=== %s\n", pnode->path);
				p = &(*p)->rb_left;
			}
			else if (res > 0)
			{
				//printf ("%s ===>\n", pnode->path);
				p = &(*p)->rb_right;
			}else
			{
				find = 1;
				pn = pnode;
				break;
				//var tree exists
			}
		}
	}
	//find path tree, then insert var into var tree
	if (find)
	{
		//printf ("path "GREEN"%s"NONE" exists\n", path);	
	}else
	{
		//malloc path_node
		pn = (path_node_t*) malloc(sizeof(path_node_t));
		if (NULL == pn)
		{
			fprintf (stderr, "%s-%d:malloc failed\n", __FILE__, __LINE__);
			return -1;
		}
		memset (pn, 0, sizeof(path_node_t));
		//malloc path
		pn->path = (char*)malloc (strlen(path)+1);
		if (!pn->path)
		{
			free (pn);
			fprintf (stderr, "%s-%d:malloc failed\n", __FILE__, __LINE__);
			return -1;

		}
		//memset (pn->path, 0, strlen(path)+1);
		memset (pn->path, 0, strlen(path)+1);
		strncpy (pn->path, path, strlen(path));
		
		rb_link_node (&pn->node, parent, p);
		rb_insert_color (&pn->node, &bin_tree);

		//the first time
		//pn->elem_node.var_name = (char*)malloc (strlen(name)+1);
		pn->elem_node.var_name = (char*)malloc (strlen(name)+1);
		if (!pn->elem_node.var_name)
		{
			fprintf (stderr, "%s-%d:malloc elem_node_t->var_name failed\n", __FILE__, __LINE__);
			//free (pe);
			return -1;

		}
		memset (pn->elem_node.var_name, 0, strlen(name)+1);
		strcpy (pn->elem_node.var_name, name);
		//malloc for var_value
		pn->elem_node.var_value = (char*)malloc (strlen(value)+1);
		if (!pn->elem_node.var_value)
		{
			fprintf (stderr, "%s-%d:malloc elem_node_t->var_value failed\n", __FILE__, __LINE__);
			free (pn->elem_node.var_name);
			//free (p);
			return -1;

		}
		memset (pn->elem_node.var_value, 0, strlen(value)+1);
		strcpy (pn->elem_node.var_value, value);
	
		rb_link_node (&pn->elem_node.node, NULL, &pn->elem_tree.rb_node);
		rb_insert_color (&pn->elem_node.node, &pn->elem_tree);
		//printf ("adding path:"RED"%s"NONE"\n", path);
		return 1;

	}//end of else	

	//insert var in its element tree
	 p = &(pn->elem_tree.rb_node);
	
	while (*p)
	{
		int8_t res = 0;
		parent = *p;
		
		enode = (elem_node_t *) rb_entry ((struct rb_node *)parent, elem_node_t, node);
		res = strcmp (name, enode->var_name);
		//printf ("%s===%s\n", enode->var_name, enode->var_value);
		if (res < 0 )
		{
			 //printf ("<---\n");
			 p = &(*p)->rb_left;
		}
		else if (res > 0)
		{
			 //printf ("--->\n");
			 p = &(*p)->rb_right;
		}else
		{
			//printf ("var name find:%s-%s-%s\n", path, name, value);
			return 1;
		}
			
	}
	pe = (elem_node_t*)malloc (sizeof(elem_node_t));
	if (NULL == pe)
	{
		fprintf (stderr, "%s-%d:malloc elem_node_t failed\n", __FILE__, __LINE__);
		return -1;
	}
	memset (pe, 0, sizeof(elem_node_t));
	//malloc for var_name
	pe->var_name = (char*)malloc (strlen(name)+1);
	if (!pe->var_name)
	{
		fprintf (stderr, "%s-%d:malloc elem_node_t->var_name failed\n", __FILE__, __LINE__);
		free (pe);
		return -1;

	}
	memset (pe->var_name, 0, strlen(name)+1);
	strncpy (pe->var_name, name, strlen(name));
	//malloc for var_value
	pe->var_value = (char*)malloc (strlen(value)+1);
	if (!pe->var_value)
	{
		fprintf (stderr, "%s-%d:malloc elem_node_t->var_value failed\n", __FILE__, __LINE__);
		free (pe->var_name);
		free (pe);
		return -1;

	}
	memset (pe->var_value, 0, strlen(value)+1);
	strncpy (pe->var_value, value, strlen(value));
	
	rb_link_node (&pe->node, parent, p);
	rb_insert_color (&pe->node, &pn->elem_tree);
	
	//printf ("store: <"CYAN"%s"NONE","GREEN"%s"NONE","YELLOW"%s"NONE">\n", path, name, value);
	return 1;
}

//search var node
int32_t		search_var (const char* path, const char* name, char* szbuf)
{

	struct rb_node ** p = NULL;
	path_node_t  * pn = NULL, *pnode = NULL;
	elem_node_t   *enode = NULL;
	struct rb_node * parent = NULL;
	u_int8_t find = 0;


	p = &bin_tree.rb_node;
	
	//try
	while (*p)
	{
		int8_t res = 0;
		parent = *p;
		pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);
		
		res = strcmp (path, pnode->path);
		//compare
		if (res < 0)
		{
			//printf ("<=== %s\n", pnode->path);
			p = &(*p)->rb_left;
		}
		else if (res > 0)
		{
			//printf ("%s ===>\n", pnode->path);
			p = &(*p)->rb_right;
		}else
		{
			find = 1;
			pn = pnode;
			break;
			//var tree exists
		}
	}
	if (!find) 
	{
		fprintf (stderr, "can not find path:%s\n", path);
		return 0;
	}
	//printf ("%s-%d find path:%s\n", __FILE__, __LINE__, path);
	p = &(pn->elem_tree.rb_node);
	
	while (*p)
	{
		int8_t res = 0;
		parent = *p;
		
		enode = (elem_node_t *) rb_entry ((struct rb_node *)parent, elem_node_t, node);
		//printf ("%s==%s\n", enode->var_name, enode->var_value);
		res = strcmp (name, enode->var_name);
		if (res < 0 )
		{
			 //printf ("<---\n");
			 p = &(*p)->rb_left;
		}
		else if (res > 0)
		{
			 //printf ("--->\n");
			 p = &(*p)->rb_right;
		}else
		{
			strcpy(szbuf, enode->var_value);
			//printf ("search [%s-%s-%s]\n", path, name, szbuf);
			return 1;
		}
			
	}
	fprintf (stderr, "can not find [%s-%s]\n", path, name);
	return 0;
}

//added by duanjigang@2011-10-09 15:30 for readline completion task
//search red-black tree to get help message

u_int8_t   cm_rbshow(const char* ppath, const cmos_config_t * cfg)
{
	struct rb_node ** p = NULL;
	path_node_t  * pn = NULL, *pnode = NULL;
	struct rb_node * parent = NULL;
	u_int8_t find = 0;
	char path[1024] = {0};
	strncpy (path, ppath, strlen(ppath));
	char * tail = path + strlen(path) - 1;
	while(*tail == '/')
	{
		*tail = '\0';
		tail--;
	}

	p = &bin_tree.rb_node;
	
	//try
	while (*p)
	{
		int8_t res = 0;
		parent = *p;
		pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);
		
		res = strcmp (path, pnode->path);
		//compare
		if (res < 0)
		{
			//printf ("<=== %s\n", pnode->path);
			p = &(*p)->rb_left;
		}
		else if (res > 0)
		{
			//printf ("%s ===>\n", pnode->path);
			p = &(*p)->rb_right;
		}else
		{
			find = 1;
			pn = pnode;
			//printf ("find:%s\n", pn->path);
			break;
			//var tree exists
		}
	}
	
	
	
	if (!find)
	{
		return RET_OK;
		fprintf (stderr, "not find %s\n", path);
	}

	//p = &(*p)->rb_right;
	if (!p) return RET_OK;
	
	parent = *p;
	//print all its brothers...	
	while (parent)
	{
		char * pp = 0;
		pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);	
		if (strcmp(pnode->path, path) > 0)
		{
			pp = strstr (pnode->path, path);
			if (pp)
			{
				char * start =  pp + strlen(path)+1;
				char desc[256] = {0};
				char maint[128] = {0};
				char type[10] = {0};
				char enable[5] = {0};
				pp = strchr (start, '/');
				if (pp) goto LOOP;
				if (strcmp(start, "package") == 0) goto LOOP;
				//printf ("%s-%s\n", start, pnode->path);
				if (search_var(pnode->path, "type", type) < 0 )goto LOOP;
				if (search_var(pnode->path, "maintanier", maint) < 0 )goto LOOP;
				if (search_var(pnode->path, "enable", enable) < 0) goto LOOP;
				search_var(pnode->path, "desc", desc);
				if (strcmp(enable, "1") < 0) goto LOOP;	
				if (strcmp(type, "dir") == 0)
				{
					if (cfg->color_on)
					printf (LIGHT_BLUE"%-15s"NONE"\t"YELLOW"%-20s"NONE"\t"CYAN"%s"NONE"\r\n", 
					start, maint, desc);	
					else
					printf ("%-15s\tp\t%-20s\t%s\r\n", start, maint, desc);	
				}
				else
				{
					if (cfg->color_on)	
					printf (GREEN"%-15s"NONE"\t"YELLOW"%-20s"NONE"\t"CYAN"%s"NONE"\r\n", 
						start, maint, desc);	
					else
					printf ("%-15s\t-\t%-20s\t%s\r\n", start, maint, desc);	
				}
			}
		}
		LOOP:
		parent = rb_next (parent);
	}


	if(0)
	{
		get_path_node_var (0, 0, 0);
	}
	return RET_OK;
}

int32_t get_path_node_var (path_node_t * pn, const char* name, char* value)
{
		
	struct rb_node ** p = NULL;
	elem_node_t   *enode = NULL;
	p = &(pn->elem_tree.rb_node);
	struct rb_node * parent = NULL;
	u_int32_t len = strlen(name);
	while (*p)
	{
		int8_t res = 0;
		parent = *p;
		
		enode = (elem_node_t *) rb_entry ((struct rb_node *)parent, elem_node_t, node);
		//printf ("%s==%s\n", enode->var_name, enode->var_value);
		res = strncmp (name, enode->var_name, len);
		if (res < 0 )
		{
			 //printf ("<---\n");
			 p = &(*p)->rb_left;
		}
		else if (res > 0)
		{
			 //printf ("--->\n");
			 p = &(*p)->rb_right;
		}else
		{
			strcpy(value, enode->var_value);
			//printf ("search [%s-%s-%s]\n", path, name, szbuf);
			return 1;
		}
			
	}
	return 0;
}
/*
char * command_generator (const char* text, int state)
{
	
	char szbuf[1024] = {0};
	char * p = rl_line_buffer;
	char ret[1024] = {0};
	//char total[1024] = {0};
	u_int8_t find_root = 0;
	char * index = 0;
	if (!state)
	{
	}
	
	if (!g_current_path || !g_plugin_path)
	{
		return (char*)NULL;
	}
	strcpy (ret, rl_line_buffer);
	p = stripwhite (ret);
	if (!*p) return (char*) NULL;

	while(p && (*p=='/'))
	{
		find_root = 1;
		p++;
	}
	if (!p && !find_root) return (char*)NULL;
	while(isspace(*p)&& p )p++;
	if (!p && !find_root) return (char*)NULL;
	
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
	//printf ("[%s]\n", szbuf);
		
	//find szbuf on the rbtree
	u_int8_t find = 0;
	u_int16_t len = strlen (szbuf);

	if (!phead) phead = &bin_tree.rb_node;
	
	//try
	while (*phead)
	{
		int8_t res = 0;
		parent = *phead;
		pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);
		//printf ("==>%s-%s\n", pnode->path, szbuf);	
		res = strncmp (szbuf, pnode->path, len);
		
		//compare
		if (res < 0)
		{
			phead = &(*phead)->rb_left;
		}
		else if (res > 0)
		{
			phead = &(*phead)->rb_right;
		}else
		{
			find = 1;
			phead = &(*phead)->rb_right;
			
			if (strlen(pnode->path) == len)
			{
				phead = &(*phead)->rb_right;
				if (phead)
				{
					parent = *phead;
					pnode = 
					(path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);
				}else
				{
					return NULL;
				}
			}
			
			pn = pnode;
			//break;
		}
	
	
		if (!find)
		{
			return (char*)NULL;
		}
	
		parent = *phead;
	}

	while (parent)
	{
		char * pp = 0;
		pnode = (path_node_t *)rb_entry ((struct rb_node *)parent,  path_node_t, node);	
		
		if (strncmp(pnode->path, szbuf, len) >= 0)
		{
			pp = strstr (pnode->path, szbuf);
			if (pp)
			{
				char * sret = 0;
				//char * start =  pp + strlen(szbuf)+1;
				char name[128] = {0};
				char enable[5] = {0};
				//pp = strchr (start, '/');
				//if (pp) goto LOOP1;
				if (search_var(pnode->path, "name", name) < 0 )goto LOOP1;
				if (search_var(pnode->path, "enable", enable) < 0) goto LOOP1;
				if (strcmp(enable, "1") < 0) goto LOOP1;	
				sprintf (total, "%s %s", rl_line_buffer, name);			
				sret = strdup (total);
				parent = rb_next (parent);
				return sret;
			}
		}
		LOOP1:
		parent = rb_next (parent);
	}
	return (char*)NULL;
}
*/

