#include "IniHelper.h"
#include <stdlib.h>
#define MAX(a, b) ((a) > (b) ? (a):(b))
#define MIN(a, b) ((a) < (b) ? (a):(b))
CIniHelper::CIniHelper(const char* file)
{
	m_fp = fopen(file, "r");
	m_pos = 0;
	if(m_fp == NULL)
	{
		m_Error = 1;
	}else
	{
		m_Error = 0;
	}
}

CIniHelper::~CIniHelper()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

int CIniHelper::ReadSection(section & pSec)
{
	if(m_Error) 
	{
		return 0;
	}
	char szLine[1024] = {0};
	int findSec = 0;
	memset(&pSec, '\0', sizeof(pSec));
	while(fgets(szLine, 1023, m_fp))
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
				
				memcpy(pSec.title, start, MIN((end - start + 1), SEC_TITLE));
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
			fseek(m_fp, -1*len, SEEK_CUR);
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
		memcpy(pSec.entry_list[pSec.entry_number].var_name, 
			name_left, 
			name_right - name_left +1);
		memcpy(pSec.entry_list[pSec.entry_number].var_value, 
			value_left, 
			value_right - value_left + 1);
		int len = strlen(pSec.entry_list[pSec.entry_number].var_value);		
		char ch = pSec.entry_list[pSec.entry_number].var_value[len - 1];
		if ( (ch == '\n') || (ch == '\t') || (ch == '\r') ||(ch == '\0'))
		{
			pSec.entry_list[pSec.entry_number].var_value[len - 1] = '\0';
		}
		memset(szLine, '\0', 1024);
		pSec.entry_number++;
	}
	return findSec;
}

int CIniHelper::ErrorOccurence()
{
	return m_Error;
}

char* CIniHelper::ReadString(char *szSec, char *szName, const char *szDefault)
{
	memset(m_strRet, 0, VAR_VAL);
	
	if(!szSec || !szName)
	{
		sprintf(m_strRet, "%s", szDefault);
		return m_strRet;
	}
	if(m_Error)
	{
		return NULL;
	}
	//store read position of the file
	m_pos = ftell(m_fp);
	//go back to head of the data file
	fseek(m_fp, 0L, SEEK_SET);
	section sc;
	memset(&sc, 0, sizeof(sc));
	while(ReadSection(sc))
	{
		if(strncasecmp(sc.title, szSec, strlen(sc.title)) != 0)
		{
			continue;
		}
		
		
		
		for(int i = 0; i < sc.entry_number; i++)
		{
			if(strncasecmp(sc.entry_list[i].var_name, szName, strlen(sc.entry_list[i].var_name)) == 0)
			{
				strncpy(m_strRet, 
					sc.entry_list[i].var_value,
					MIN((VAR_VAL),(strlen(sc.entry_list[i].var_value))));
				break;
			}
		}
		break;
	}
	if(strlen(m_strRet) == 0)
	{
		strncpy(m_strRet, szDefault, VAR_VAL);
	}
	fseek(m_fp, m_pos*1L, SEEK_SET);
	return m_strRet;

}

int CIniHelper::ReadInt(char *szSec, char *szName, int nDefault)
{
	if(!szSec || !szName)
	{
		printf("erro parameter in Readint\n");
		return nDefault;
	}
	if(m_Error)
	{
		printf("erro parameter in Readint\n");
		return nDefault;
	}
	//store read position of the file
	m_pos = ftell(m_fp);
	//go back to head of the data file
	fseek(m_fp, 0L, SEEK_SET);
	section sc;
	memset(&sc, 0, sizeof(sc));
	m_nRet = -1;
	bool find = false;
	while(ReadSection(sc))
	{
		if(strncasecmp(sc.title, szSec, strlen(sc.title)) != 0)
		{
			continue;
		}
		
	
		
		for(int i = 0; i < sc.entry_number; i++)
		{
			if(strncasecmp(sc.entry_list[i].var_name, szName, strlen(sc.entry_list[i].var_name)) == 0)
			{
				m_nRet = atoi(sc.entry_list[i].var_value);
				find = true;
				break;
			}
		}
		break;
	}
	if(!find)
	{
		printf("not find in Readint sec:%s,name:%s\n", szSec, szName);
		m_nRet = nDefault;
	}
	
	fseek(m_fp, m_pos*1L, SEEK_SET);
		return m_nRet;
}
