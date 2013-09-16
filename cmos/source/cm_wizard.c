#include "cmos.h"

u_int8_t handle_message (const char* szline, cmos_status_t * stat, cmos_config_t * cfg)
{
	const char* p = 0;
	int16_t nret = 0;
	u_int32_t length = strlen(szline);
	u_int32_t index = 0;
	
	for (index = 0; index < length; index++)
	{
		p = szline + index;
		if (!isspace(*p))
		{
			break;
		}	
	}
	
	if (!p)
	{
		return RET_OK;
	}
	if ((nret = handle_innercmd (p, stat, cfg)) >= 0)
	{
		return nret;
	}
	cfg->op_mask = OP_PLUGIN_CMD;
	return cm_rbcmd (stat, p, cfg);

	//common command
	

	return RET_OK;
}

