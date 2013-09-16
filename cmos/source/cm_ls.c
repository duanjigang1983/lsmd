#include "cmos.h"
#include "IniHelper.h"

u_int8_t   cm_ls(const cmos_status_t * stat)
{
	DIR * dir = 0;      
        struct dirent* drt = 0; 
	
	//try to open dir
	if( (dir = opendir (stat->current_dir))== NULL )
	{
		return RET_ACCESS_FAILED;
	}

	//loop this dir
	while ((drt = readdir(dir)) != NULL)
        {
		if ((strcmp(drt->d_name, ".")==0)|| (strcmp(drt->d_name, "..")==0))
                {
                        continue;
                }
		
		if ((drt->d_type == DT_SOCK) || (drt->d_type == DT_FIFO))
                {
                        continue;
                }
		//show list		
		if (drt->d_type == DT_DIR)
		{
			printf (YELLOW"%-15s"NONE"\n", drt->d_name);
		}
		else
		{
			printf (GREEN"%-15s"NONE"\t%s\n", drt->d_name, "help message");	
		}
	}	
	closedir (dir);
	return RET_OK;
}

u_int8_t   cm_show(const cmos_status_t * pstat)
{
	u_int32_t i = 0;

	for ( i = 0; i < pstat->cmd_list->cmd_num; i++)
	{
		command_t * pcmd = pstat->cmd_list->cmd_list + i;
		
		if (pcmd->type == TYPE_PKG)
		{
			printf (LIGHT_BLUE"%-15s"NONE"\t"CYAN"%s"NONE"\r\n", pcmd->name, pcmd->desc);	
		}
		else
		{
			printf (GREEN"%-15s"NONE"\t"CYAN"%s"NONE"\r\n", pcmd->name, pcmd->desc);	
		}

	}
	return RET_OK;
}


