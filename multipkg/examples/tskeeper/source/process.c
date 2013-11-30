#include "process.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
/*
for function "stat",we must include  file "sys/stat"
it is defined in both windows and linux os
*/
#include <sys/stat.h>
int init_proc_struct(struct proc_struct * proc)
{
	if(!proc)
	{
		return 0;
	}
	else
	{
		memset(proc->name, '\0', MAX_PROC_NAME);
		memset(proc->cmdline, '\0', MAX_CMD_LINE);
		proc->pid = proc->ppid = proc->status = INVALID_INTEGER;
		proc->thread = proc->price = proc->nice = INVALID_INTEGER;
		proc->size = proc->rss = proc->time = proc->uid = INVALID_ULONG;
		proc->pcpu = proc->wcpu = INVALID_DOUBLE;
		return 1;
	}
}
int fill_proc_struct(struct proc_struct* proc, const char * szPid)
{
    int pid = atoi(szPid);
    char buffer[4096], *p, cmd[20];
    int fd, len;
    if(!proc || pid <= 0)
    {
		return 0;
    }
	
    sprintf(buffer, "%d/stat", pid);
    sprintf(cmd, "%d/cmdline", pid);
    fd = open(buffer, O_RDONLY);
    if(fd == -1)
    {
		return 0;
    }
    memset(buffer, '\0', sizeof(buffer));
    len = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    if(len <= 0)
    {
		return 0;
    }
	
    proc->uid = proc_owner(pid);
    p = buffer;
    p = strrchr(p, '(');
    {
	/*
	get the position of name
        */
		char *q = strrchr(p, ')');
		int len = q - p - 1;
		if (len >= sizeof(proc->name))
		{
			len = sizeof(proc->name)-1;
		}
        memcpy(proc->name, p + 1, len);
		proc->name[len] = 0;
		p = q + 1;
    }
	
    p = skip_ws(p);
    switch (*p++)
    {
	case 'R': proc->status = 1; break;
	case 'S': proc->status = 2; break;
	case 'D': proc->status = 3; break;
	case 'Z': proc->status = 4; break;
	case 'T': proc->status = 5; break;
	case 'W': proc->status = 6; break;
    }
    proc->ppid = atoi(p);    
    p = skip_token(p);		/* skip ppid */
    p = skip_token(p);		/* skip pgrp */
    p = skip_token(p);		/* skip session */
    p = skip_token(p);		/* skip tty */
    p = skip_token(p);		/* skip tty pgrp */
    p = skip_token(p);		/* skip flags */
    p = skip_token(p);		/* skip min flt */
    p = skip_token(p);		/* skip cmin flt */
    p = skip_token(p);		/* skip maj flt */
    p = skip_token(p);		/* skip cmaj flt */
    
    proc->time = strtoul(p, &p, 10);	/* utime */
    proc->time += strtoul(p, &p, 10);	/* stime */
	
    p = skip_token(p);		       /* skip cutime */
    p = skip_token(p);		      /* skip cstime */
	
    proc->price = strtol(p, &p, 10);	/* priority */
    proc->nice = strtol(p, &p, 10);	/* nice */
	
    p = skip_token(p);		/* skip timeout */
    p = skip_token(p);		/* skip it_real_val */
    p = skip_token(p);		/* skip start_time */
#ifdef __LINUX__
    proc->size = bytetok(strtoul(p, &p, 10));	/* vsize */
    proc->rss = pagetok(strtoul(p, &p, 10));	/* rss */
#endif
#if 0
    /* for the record, here are the rest of the fields */
    p = skip_token(p);		/* skip rlim */
    p = skip_token(p);		/* skip start_code */
    p = skip_token(p);		/* skip end_code */
    p = skip_token(p);		/* skip start_stack */
    p = skip_token(p);		/* skip sp */
    p = skip_token(p);		/* skip pc */
    p = skip_token(p);		/* skip signal */
    p = skip_token(p);		/* skip sigblocked */
    p = skip_token(p);		/* skip sigignore */
    p = skip_token(p);		/* skip sigcatch */
    p = skip_token(p);		/* skip wchan */
#endif  
    fd = open(cmd, O_RDONLY);
    if(fd != -1) 
    {
		read(fd, proc->cmdline, sizeof(proc->cmdline));
		close(fd);   
    }
	
}

int proc_owner(int pid)
{
	struct stat sb;
	char buffer[32];
	sprintf(buffer, "%d", pid);
    if (stat(buffer, &sb) < 0)
	{
		return -1;
	}
	else
	{
		return sb.st_uid;
	}
}
int get_proc_list(struct proc_struct* array, int size)
{
	
	int counter = 0;
	DIR * dp;
	char * dir = "/proc";
	struct dirent * dirp;
	struct proc_struct* proc;
	if(!array || size <= 0)
	{
		return 0;
	}
	chdir(dir);
	if( (dp = opendir(dir))== NULL)
	{
		
		return 0;    
	}
	while ((dirp = readdir(dp)) != NULL && counter < size)
	{
		
		char data[30];
		sprintf(data, "%s", dirp->d_name);  
		if((IsDigit(data)))
		{
			proc = &(array[counter]);
			init_proc_struct(proc);
			
			proc->pid = atoi(dirp->d_name);
			fill_proc_struct(proc, dirp->d_name);
			counter++;                    
		}
	}
	closedir(dp);
	return counter;
}
char* print_proc(const struct proc_struct* proc, int flag)
{
	
	static char msg[1024];
	if(!proc)
	{
		return NULL;
	}
	memset(msg, '\0', sizeof(msg));
	/*
	sprintf(msg, "%s, %s, %d, %d, %d, %s",
	proc->name,
	proc->cmdline,
	proc->pid,
	proc->ppid,
	proc->uid,
	proc_state_names[proc->status]);
	*/
	sprintf(msg, "%s, %s, %s, %d, %d", 
		proc->name, 
		proc->cmdline, 
		proc->param,
		proc->new_status,
		proc->dead_time);
	if(flag)
	{
		printf("%s\n", msg);
	}
	return msg;
}
int proc_equal(const struct proc_struct* src, const struct proc_struct* dest)
{
	if(!src || !dest)
	{
		printf("Invalid parameter in proc_equal\n");
		exit(0);	
	}
	else
	{
		int len,len1,len2;
		len1 = strlen(src->name);
		len2 = strlen(dest->name);
		len = (len1 >= len2) ? len2 : len1;
		return(
		(strncmp(src->name, dest->name, len) == 0)/*&&
												  (strcmp(src->cmdline, dest->cmdline) == 0)&&
												  (src->pid  == dest->pid)&&
												  (src->uid  == dest->uid)&&
												  (src->ppid == dest->ppid)*/)? 1 : 0;
	}
}
int proc_find(struct proc_struct* entry, 
              const struct proc_struct* proc_list, 
              int size)
{
	if(!entry || !proc_list || size <= 0)
	{
		return -1;	
	}else
	{
		int i;
		for(i = 0; i < size; i++)
		{
			if(proc_equal(entry, &(proc_list[i])))
			{
			 	entry->pid = proc_list[i].pid;	
				return i;	
			}	
		}	
		return -1;	
	}
}
int get_proc_list_from_file(
							struct proc_struct* array, 
							int size, 
							const char* file)
{
	int nRet = 0;
	if(!array || (size <= 0) || !file)
	{
		printf("invalid parameter\n");
		return 0;
	}
	char line[4096];
	FILE* fp = fopen(file, "r");
	if(!fp) 
	{
		printf("open file %s fail\n", file);
		return 0;
	}
	//fgets(line, 4095, fp);	
	memset(line, 0, 4096);
	while(fgets(line, 4095, fp)&& nRet < size)
	{
		char* q;
		q = skip_ws(line);
		if(strlen(q) <= 0) continue;
		if(*(q) == '#')continue;
		if(!invalidline(q)) continue;
		if(parse(&(array[nRet]), q)) ++nRet;
	}
	fclose(fp);
	return (nRet);
}
int parse(struct proc_struct* node,  char* line)
{
	if(!node || !line) return 0;
	char* head, *tail;
	char* name = strstr(line, "name");
	char* path = strstr(line, "path");
	char* param = strstr(line, "param");
	char* waittime = strstr(line, "waittime");
	char* runtime = strstr(line, "runtime");
	char szTime[20];
	//printf("line:%s", line);
	if((name == NULL)||(path == NULL)||(param == NULL)||(waittime == NULL)||(runtime==NULL)) 
	{
		printf("error content in file: %s\n", line);
		return 0;
	}
	head = name;
	while(*head && (*head != '"') && (*head != ';'))++head;	
	if(!head || (*head) == ';')
	{
		printf("error name: %c\n", *head);
		return 0;
	}
	tail = head + 1;
	while(*tail && (*tail != '"') && (*tail != ';'))++tail;
	if(!tail || (*tail) == ';')
	{
		printf("error name: %c\n", *head);
		return 0;
	}
	tail--;
	head++;
	memcpy(node->name, head, tail - head + 1);
	head = path;
	while(*head && (*head != '"') && (*head != ';'))++head;	
	if(!head || (*head) == ';')return 0;
	tail = head + 1;
	while(*tail && (*tail != '"') && (*tail != ';'))++tail;
	tail--;
	head++;
	memcpy(node->cmdline, head, tail - head + 1);
	
	head = param;
	while(*head && (*head != '"') && (*head != ';'))++head;	
	if(!head || (*head) == ';')return 0;
	tail = head + 1;
	while(*tail && (*tail != '"') && (*tail != ';'))++tail;
	tail--;
	head++;
	memcpy(node->param, head, tail - head + 1);
	
	head = waittime;
	while(*head && (*head != '"') && (*head != ';'))++head;	
	if(!head || (*head) == ';')return 0;
	tail = head + 1;
	while(*tail && (*tail != '"') && (*tail != ';'))++tail;
	tail--;
	head++;
	memset(szTime, 0, 20);
	memcpy(szTime, head, tail - head + 1);
	node->dead_counter = atoi(szTime);
	node->dead_time = 0;
	//added by duanjigang @2008-07-28 23:15 -- start
	head = runtime;
	while(*head && (*head != '"')&&(*head != ';')) ++head;
	if(!head || (*head) == ';')return 0;
	tail = head + 1;
	while(*tail && (*tail != '"') && (*tail != '>'))++tail;
        tail--;
        head++;
        memset(szTime, 0, 20);
        memcpy(szTime, head, tail - head + 1);
        node->live_counter = atoi(szTime);
	node->live_time=0;
	//added by duanjigang@2008-07-28  -- end
	node->new_status = 0;
	/*
		printf("================%s:%s:%s:%d:%d:%d\n", 
		node->name, 
		node->cmdline, 
		node->param, 
		node->new_status, 
		node->dead_counter,
		node->live_counter);
	*/
	return 1;	
}
int startProc(const struct proc_struct* proc)
{
    if(!proc) 
	{
		return 0;
	}
	if(strlen(proc->cmdline) <= 0)
	{
		return 0;
	}
	int pid = fork();
	if(pid == 0)
	{
		const char* szParam[] = {proc->cmdline, proc->param, NULL};
		pid = fork();
		if(pid == 0)
		{
			execvp(proc->cmdline, szParam);
			exit(0);
		}else
		{
			exit(0);
		}
		
	}else
	{
		wait(NULL);
		return 1;		
	}	
}
int  killProc(const struct proc_struct* proc)
{
                int status = kill(proc->pid, SIGINT);
                if(status != 0)
                {
                        char msg[128] = {0};
                        printf(msg, "kill task %s fail\n", proc->name);
			return -1;
                }
		return 1;
}
