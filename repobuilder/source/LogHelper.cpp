#include "LogHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#include <unistd.h>
static int g_pid = 0;

CLogHelper::CLogHelper()
{
}

CLogHelper::~CLogHelper()
{
	
}

void CLogHelper::LogMessage(const char *szFile, const char* fmt,...)
{
	FILE * fp = NULL;
	
	if(!szFile)
	{
		return;
	}
	
	if( (fp = fopen(szFile, "a")) == NULL)
	{
		return;
	}
	struct tm * newtime;
	char s[32]= {0};
	time_t ct;
	time(&ct);
	if (!g_pid)
	g_pid = getpid();	
	newtime = gmtime(&ct);
   	sprintf( s,"<%04d-%02d-%02d %02d:%02d:%02d %d>", newtime->tm_year + 1900, (newtime->tm_mon + 1)%12, newtime->tm_mday,
                                (newtime->tm_hour+8)%24, newtime->tm_min, newtime->tm_sec, g_pid);
	fprintf(fp, "%s: ", s);
	va_list args;
	va_start(args, fmt);
	vfprintf(fp, fmt, args);
	va_end(args);	
	fclose(fp);
}
