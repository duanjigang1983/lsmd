#ifndef _PROCESS_H_
#define _PROCESS_H_
#define PROCFS "/proc"
#define MAX_PROC_NAME 128
#define MAX_CMD_LINE 256
#define PROC_STATUS 6
#define INVALID_INTEGER -1
#define INVALID_ULONG 0
#define INVALID_DOUBLE 0
#include <string.h>

static inline char *
skip_ws(const char *p)
{
    while (isspace(*p))
	{
		p++;
    }
	return (char *)p;
}
static inline char * skip_token(const char *p)
{
    while (isspace(*p)) p++;
    while (*p && !isspace(*p))
	{
		p++;
    }
	return (char *)p;
}
static inline int invalidline(const char* line)
{
	if(!line)
	{
		return 0;
	}
	if(!(strstr(line, "name")&& strstr(line, "param") && strstr(line, "path")))
	{
		return 0;
	}
	return 1;	
}
/*
added by duanjigang -- START 2005/12/24
*/
static inline char *
strhd(const char*pp)
{
	int i = 0; 
	static char buffer[100], *p;
	p =(char*)pp;  
	memset(buffer,0,100);
	while(isspace(*p) || *p == '\n')
	{
		p++;
	}
	while(*p && !isspace(*p))
	{
		buffer[i] = *p;
		i++;
		p++;
	}
	buffer[i] = '\0';
	return buffer;
}
/*
added by duanjigang -- END 2005/12/24
*/
static  int IsDigit(char a[])
{
	
	
	int size,i;
	size = strlen(a);
	if(size == 0)
	{
		return 0;
	}
	for(i = 0; i< size; i++)
	{
		if(a[i] <'0' || a[i] > '9')
		{
			return 0;
		}
	}
	return 1;
}
static char * proc_state_names[PROC_STATUS + 1] =
{
    " unknow ",   " running ", 
		" sleeping ", " uninterruptable ",
		" zombie ",   " stopped ", " swapping "
};

struct proc_struct
{
	char name[MAX_PROC_NAME];
	char cmdline[MAX_CMD_LINE];
	char param[MAX_CMD_LINE];
	int pid;
	int uid;
	int ppid;
	int status;
	int thread;//do not get
	int price, nice; //do not 
	unsigned long size, rss; //do not 
	unsigned long time;//do not
	double pcpu, wcpu;// do not
	int new_status;
	int dead_time;
	int live_time;
	int dead_counter;
	int live_counter;
};

int proc_find(struct proc_struct* entry, const struct proc_struct* proc_list, int size);
int proc_equal(const struct proc_struct* proc1,const struct proc_struct* proc);
int proc_owner(int  pid);
int init_proc_struct(struct proc_struct* proc);
int fill_proc_struct(struct proc_struct* proc, const char * szPid);
int get_proc_list(struct proc_struct* array, int size);
int get_proc_list_from_file(struct proc_struct* array, int size, const char* file);
int parse(struct proc_struct* array,  char* line);
int startProc(const struct proc_struct* proc);
char* print_proc(const struct proc_struct* proc, int flag);
int  killProc(const struct proc_struct* proc);
#endif

