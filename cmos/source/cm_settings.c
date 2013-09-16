#include "cmos.h"

void show_settings (const cmos_config_t * cfg);
u_int8_t change_settings ( cmos_config_t * cfg, const char* data);

u_int8_t help_set (const char*data,  cmos_status_t * stat,   cmos_config_t * cfg)
{
	const char * p = data;
	while (*p && isspace(*p))
	{
		p++;
	}
	if (!*p)
	{
		//printf ("no settins\n");
		show_settings (cfg);
		return RET_OK;
	}
	change_settings (cfg, p);
	//printf ("%s\n", p);
	return RET_OK;
}

void show_settings (const cmos_config_t * cfg)
{
	printf ("\tkeep_alive\t%s\n", cfg->keep_alive ? "on" : "off");	
	printf ("\tcolor\t\t%s\n", cfg->color_on ? "on" : "off");	
	printf ("\tinput_time_out\t%-3u\n", cfg->input_time_out);	
	printf ("\trun_time_out\t%-5u\n", cfg->run_time_out);	
}
const char* strnext (const char* szdata)
{
	const char * p = szdata;
	while (*p && !isspace(*p)) p++;
	if (!*p) return NULL;
	while(*p && isspace(*p))p++;
	if (!*p) return NULL;
	return p;
}
u_int8_t change_settings (cmos_config_t * cfg, const char* data)
{
	//change keep_alive
	if (strncmp (data, "keep_alive", 10) == 0)
	{
		const char* p = strnext (data);
		if (!p)
		{
			printf ("\tkeep_alive\t%s\n", cfg->keep_alive ? "on" : "off");	
			return RET_OK;
		}		
		//printf ("[%s]\n", p);
		if (strcmp(p, "on") && strcmp(p, "off"))
		{
			printf ("\tkeep_alive\t%s\n", cfg->keep_alive ? "on" : "off");	
			return RET_OK;
		}
		cfg->keep_alive = (strcmp(p, "on") == 0) ? 1 : 0;
		return RET_OK;	
	}

	if (strncmp (data, "input_time_out", 14) == 0)
	{
		const char* p = strnext (data);
		int32_t tt = 0;
		if (!p)
		{
			printf ("\tinput_time_out\t%3u\n", cfg->input_time_out);	
			return RET_OK;
		}		
		tt = atoi (p);
		if ((tt > 0) && (tt <= 3600))
		{
			cfg->input_time_out  = (u_int16_t)tt;
			return RET_OK;
		}
		return RET_OK;	
	}
	//added for run time out by jigang.djg@2012-04-29 --start
	if (strncmp (data, "run_time_out", 12) == 0)
	{
		const char* p = strnext (data);
		u_int32_t tt = 0;
		if (!p)
		{
			printf ("\trun_time_out\t%5u\n", cfg->run_time_out);	
			return RET_OK;
		}		
		tt = strtoul (p, 0, 10);
		if ((tt > 0) && (tt <= MAX_RUN_TIMEOUT))
		{
			cfg->run_time_out  = tt;
			return RET_OK;
		}
		return RET_OK;	
	}
	//added for run time out by jigang.djg@2012-04-29 --finish


	//change color switch
	//added by jigang.djg@2012-01-09 --start
	if (strncmp (data, "color", 5) == 0)
	{
		const char* p = strnext (data);
		if (!p)
		{
			printf ("\tcolor\t%s\n", cfg->color_on ? "on" : "off");	
			return RET_OK;
		}		
		//printf ("[%s]\n", p);
		if (strcmp(p, "on") && strcmp(p, "off"))
		{
			printf ("\tcolor\t%s\n", cfg->color_on ? "on" : "off");	
			return RET_OK;
		}
		cfg->color_on = (strcmp(p, "on") == 0) ? 1 : 0;
		return RET_OK;	
	}
	//added by jigang.djg@2012-01-09 --finish

	return RET_OK;
}
