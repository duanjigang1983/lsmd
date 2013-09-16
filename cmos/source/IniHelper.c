#include "IniHelper.h"
#include <stdlib.h>
#define MAX(a, b) ((a) > (b) ? (a):(b))
#define MIN(a, b) ((a) < (b) ? (a):(b))

typedef struct
{
	FILE * m_fp ; //file pointer
	int  m_Error; // error flag
	long m_pos;
	char m_strRet[VAR_VAL];
	int  m_nRet;
}ini_t;
/*
int init_ini(const char* file)
{
	m_fp = fopen(file, "r");
	m_pos = 0;

	if(m_fp == NULL)
	{
		m_Error = 1;
		return -1;
	}else
	{
		m_Error = 0;
		return 1;
	}
}*/

void* init_ini(const char* file)
{
	ini_t * ini = (ini_t*)malloc (sizeof(ini_t));
	if (!ini)
	{
		printf ("%s-%d:malloc ini_t failed\n", __FILE__, __LINE__);
		return NULL;
	}
	memset (ini, 0, sizeof(ini_t));
	ini->m_fp = fopen(file, "r");
	ini->m_pos = 0;

	if(ini->m_fp == NULL)
	{
		ini->m_Error = 1;
		free (ini);
		return NULL;
	}else
	{
		ini->m_Error = 0;
		return ini;
	}
	return NULL;
}

void 	close_ini(void * data)
{
	ini_t * ini = (ini_t*) data;
	if(ini->m_fp)
	{
		fclose(ini->m_fp);
		ini->m_fp = NULL;
		free(ini);
	}
}

int read_section(void* data, section * pSec)
{
	ini_t * ini = (ini_t*)data;
	if(ini->m_Error) 
	{
		return 0;
	}
	char szLine[1024] = {0};
	int findSec = 0;
	memset(pSec, '\0', sizeof(section));
	while(fgets(szLine, 1023, ini->m_fp))
	{
		if(szLine[strlen(szLine)-1] == '\n')
		{
			szLine[strlen(szLine)-1] = '\0';
		}
		
		char* p = szLine;
		while((*p != '\0') && isspace(*p))
		{
			p++;
		}
		
		if((*p == '\0')||(*p == '#'))
		{
			memset(szLine, '\0', 1024);
			continue;
		}
		
		if(!findSec)
		{
			
			
			if(*p != '[')
			{
				memset(szLine, '\0', 1024);
				continue;
			}
			
			char* end = strstr(p + 1, "]");
			char* start = p + 1;
			//没找到成对的括号
			if(!end)
			{
				memset(szLine, '\0', 1024);
				continue;
			}
			end--;
			while(isspace(*start)&&(start < end)) start++;
			while(isspace(*end) && (end > start)) end--;
			//拷贝title
			if(end - start >= 0)
			{
				
				memcpy(pSec->title, start, MIN((end - start + 1), SEC_TITLE));
			}
			findSec = 1;
			memset(szLine, '\0', 1024);
			continue;
		}
		if(!findSec)
		{
			//printf("do not find sec\n");
			memset(szLine, '\0', 1024);
			continue;
		}
		//a new section start
		if(*p == '[')
		{
			
			long len = strlen(szLine)+2;
			fseek(ini->m_fp, -1*len, SEEK_CUR);
			break;
		}
		
		char* token = strstr(p, "=");
		//do not find sentence like "xxx = xxx"
		if(!token)
		{
		//	printf("error line:%s\n", p);
			memset(szLine, '\0', 1024);
			continue;
		}
		
		char* name_right = token - 1;
		char* name_left = p;
		char* value_left = token + 1;
		char* value_right = token+1;
		while((*value_right != '\0')&&(*value_right != '\r')&&(*value_right != '\n'))
		{
			value_right++;
		}
		while(isspace(*name_right) && (name_right != name_left))
		{
			name_right--;
		}
		
		while(isspace(*value_left) && (value_left != value_right))
		{
			value_left++;
		}
		//printf("%d = %d\n", name_right - name_left + 1, value_right - value_left + 1);
		memcpy(pSec->entry_list[pSec->entry_number].var_name, 
			name_left, 
			name_right - name_left +1);
		memcpy(pSec->entry_list[pSec->entry_number].var_value, 
			value_left, 
			value_right - value_left + 1);
		int len = strlen(pSec->entry_list[pSec->entry_number].var_value);		
		char ch = pSec->entry_list[pSec->entry_number].var_value[len - 1];
		if ( (ch == '\n') || (ch == '\t') || (ch == '\r') ||(ch == '\0'))
		{
			pSec->entry_list[pSec->entry_number].var_value[len - 1] = '\0';
		}
		memset(szLine, '\0', 1024);
		pSec->entry_number++;
	}
	return findSec;
}

int have_error(void* data)
{
	ini_t * ini = (ini_t*)data;
	return ini->m_Error;
}

char* read_string(void* data, const char *szSec, const char *szName, const char *szDefault)
{
	ini_t * ini = (ini_t*)data;
	section sc;
	memset(ini->m_strRet, 0, VAR_VAL);
	
	if(!szSec || !szName)
	{
		sprintf(ini->m_strRet, "%s", szDefault);
		return ini->m_strRet;
	}
	if(ini->m_Error)
	{
		return NULL;
	}
	//store read position of the file
	ini->m_pos = ftell(ini->m_fp);
	//go back to head of the data file
	fseek(ini->m_fp, 0L, SEEK_SET);
	memset(&sc, 0, sizeof(sc));
	while(read_section(data, &sc))
	{
		int i = 0;
		if(strncasecmp(sc.title, szSec, strlen(sc.title)) != 0)
		{
			continue;
		}
		
		
		
		for(i = 0; i < sc.entry_number; i++)
		{
			if(strncasecmp(sc.entry_list[i].var_name, szName, strlen(sc.entry_list[i].var_name)) == 0)
			{
				strncpy(ini->m_strRet, 
					sc.entry_list[i].var_value,
					MIN((VAR_VAL),(strlen(sc.entry_list[i].var_value))));
				break;
			}
		}
		break;
	}
	if(strlen(ini->m_strRet) == 0)
	{
		strncpy(ini->m_strRet, szDefault, VAR_VAL);
	}
	fseek(ini->m_fp, ini->m_pos*1L, SEEK_SET);
	return ini->m_strRet;

}

int read_int(void* data, const char *szSec, const char *szName, int nDefault)
{
	unsigned short find = 0;
	unsigned int i = 0;
	ini_t * ini = (ini_t*)data;

	if(!szSec || !szName)
	{
		printf("erro parameter in Readint\n");
		return nDefault;
	}
	if(ini->m_Error)
	{
		printf("erro parameter in Readint\n");
		return nDefault;
	}
	//store read position of the file
	ini->m_pos = ftell(ini->m_fp);
	//go back to head of the data file
	fseek(ini->m_fp, 0L, SEEK_SET);
	section sc;
	memset(&sc, 0, sizeof(sc));
	ini->m_nRet = -1;
	
	while(read_section(data, &sc))
	{
		if(strncasecmp(sc.title, szSec, strlen(sc.title)) != 0)
		{
			continue;
		}
		
	
		
		for(i = 0; i < sc.entry_number; i++)
		{
			if(strncasecmp(sc.entry_list[i].var_name, szName, strlen(sc.entry_list[i].var_name)) == 0)
			{
				ini->m_nRet = atoi(sc.entry_list[i].var_value);
				find = 1;
				break;
			}
		}
		break;
	}
	if(!find)
	{
		printf("not find in Readint sec:%s,name:%s\n", szSec, szName);
		ini->m_nRet = nDefault;
	}
	
	fseek(ini->m_fp, ini->m_pos*1L, SEEK_SET);
		return ini->m_nRet;
}
