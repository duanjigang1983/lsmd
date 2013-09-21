
#if !defined(INI__INCLUDED_)
#define INI__INCLUDED_
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#define VAR_NAME 64
#define VAR_VAL  128
#define SEC_TITLE 64
#define MAX_VAR   200
#define MAX_SEC	   500
//define a variable
typedef struct _var_entry
{
	char var_name[VAR_NAME];
	char var_value[VAR_VAL];
}var_entry;
//define a section
typedef struct _section
{
	char title[SEC_TITLE];
	var_entry entry_list[MAX_VAR];
	int		entry_number;
}section;

int 	read_int(void* data, const char* szSec, const char* szName, int nDefault);
char* 	read_string(void* data, const char* szSec, const char* szName, const char* szDefault);
int 	have_error(void * data);
int 	read_section(void* data, section * pSec);
//int 	init_ini(const char*  file);
void* 	init_ini(const char*  file);
void 	close_ini(void* data);
#endif 
