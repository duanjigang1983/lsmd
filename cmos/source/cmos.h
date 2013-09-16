#ifndef _CMOS_H_
#define _CMOS_H_
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <libgen.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/rltypedefs.h>

#include "constdef.h"
#include "colordef.h"



#define  INPUT_FLAG ">"
//#define  DATA_DIR "/usr/local/cmos/data"
//#define  DATA_DIR "data"
#define	 CTL_FILE	"cmos.ini"
#define CMD_LEN 128
#define DESC_LEN 128
#define TYPE_PKG 1
#define TYPE_CMD 2
typedef struct
{
	char name[CMD_LEN];
	char desc[DESC_LEN];
	u_int8_t enable;
	u_int8_t type;
}command_t;
#define MAX_CMD_EACH_DIR 500
typedef struct
{
	command_t cmd_list[MAX_CMD_EACH_DIR];
	u_int32_t cmd_num;
}cmd_list_t;


#define PATH_LEN 256
typedef struct
{
	char current_dir[PATH_LEN];//where we are
	char show_path[PATH_LEN];//show path
	cmd_list_t * cmd_list;
}cmos_status_t;

typedef struct 
{
	char plugin_dir	[PATH_LEN];
	u_int8_t	keep_alive;// exit cmos after each command handling
	u_int16_t	input_time_out;//how soon to exit cmos when nothing is input
	u_int32_t	op_mask;//operation mask
	u_int8_t	color_on;//print message with color --added by jigang.djg@2012-01-09
	u_int8_t	quit_flag;//added by jigang.djg@2012-02-07
	u_int32_t	run_time_out;//run timeout--added by jigang.djg@2012-04-19
}cmos_config_t;

typedef u_int8_t (*help_func_t)(const char *data, cmos_status_t * stat,   cmos_config_t * cfg);
typedef struct 
{
	char* name;
	help_func_t func;
	char* desc;
	u_int8_t printable;
}help_cmd_t;

extern u_int8_t		init_signal	(const cmos_status_t * stat);
extern u_int8_t   	cm_ls		(const cmos_status_t * stat);
extern u_int8_t   	cm_show		(const cmos_status_t * stat);
extern u_int8_t 	cm_upper 	(cmos_status_t * stat,	const cmos_config_t * cfg);
extern u_int8_t		cm_command 	(cmos_status_t * stat,	const char* szline,	const cmos_config_t * cfg);
extern u_int8_t		init_status 	(cmos_status_t *stat,	const cmos_config_t * cfg);
extern u_int8_t		cm_cmd 		(cmos_status_t * stat,	const char* szline,	const cmos_config_t * cfg);
extern u_int8_t      	cm_run_cmd	(cmos_status_t * stat,	const char* szfile,	const char* szraw,
						const cmos_config_t * cfg);
//extern u_int8_t      	cm_run_cmd	(cmos_status_t * stat,	const char* szfile,	const char* szraw);
extern u_int8_t      	cm_cat_cmd	(cmos_status_t * stat,	const char* szfile,	const char* szraw);
extern u_int8_t		cm_rbcmd 	(cmos_status_t * stat,	const char* szline,	const cmos_config_t * cfg);
extern u_int8_t   	handle_message 	(const char* szline,	cmos_status_t * stat,	 cmos_config_t * cfg);
extern int16_t   	handle_innercmd	(const char* szline,	cmos_status_t * stat,	 cmos_config_t * cfg);
extern u_int8_t   	cm_rbshow	(const char* path, const cmos_config_t * cfg);
extern int32_t		load_bins 	(const char* path,	cmd_list_t *list);
extern int32_t		load_plugins 	(const char* path,	cmos_status_t * stat,	const cmos_config_t * cfg);
extern int32_t 		insert_var 	(const char* path,	const char* name,	const char* value);
extern int32_t		search_var 	(const char* path,	const char* name,	char* szbuf);
extern int32_t 		get_userid (const char* name, uid_t * uid);
//extern int8_t		cm_parseparam 	(int argc, 		char* argv[], 		char* szbuf);
extern int8_t		cm_parseparam 	(int argc, 		char* argv[], 		cmos_config_t * cfg);
extern void  		print_help	(void);
extern int32_t		read_line	(char* szbuf, u_int32_t length, const cmos_config_t * cfg);

void			init_readline	(cmos_status_t * stat, cmos_config_t * config);
extern char * 		stripwhite 	(char * string);
extern	u_int8_t	add_completion	(const char* path, u_int8_t valid);
extern void		sort_completion (void);
#endif
