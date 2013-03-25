#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "dbpub.h"

#define CONFIG 1

char *TSTRUTILS::str_dup(const char *src)
{
	if (!src) return NULL;

	int iLength = strlen(src) + 1;
	char *dst = (char *) new char[iLength];

	if (!dst) return NULL;

	strcpy(dst, src);

	return(dst);
}

// 释放被复制的字符串
void TSTRUTILS::free_dup(char *&src)
{
    if(src)
    {
        delete [] src;
        src = NULL;
    }

    return;
}

char *TSTRUTILS::ltrim(char *str)
{
	char *pstr = str;
	
 	if(!pstr) return (NULL);

 	for(;pstr;)
	{
	   if (*pstr == ' ')
	      pstr++; 
	   else
	      return((char *)pstr);	
	}

	return(NULL);
}

void TSTRUTILS::rtrim(char *str)
{
	int len;	

 	len  = strlen(str) - 1;

   for(;(len >= 0) && ((str[len] == ' ') || (str[len] == '\t') || (str[len] == '\n'));)
 		len--;

 	str[len + 1] = '\0';
}


char *TSTRUTILS::trim(char *str)
{
	if(!str) return NULL;

 	int len  = strlen(str) - 1;
	
	//delete right space 
	bool bTrimRight = false;
 
	for(;(len >= 0) && ((str[len] == ' ') || (str[len] == '\t') || (str[len] == '\n'));)
	{
        len--;
		bTrimRight = true;
	}
    
	if(bTrimRight) str[len + 1] = '\0';

	char *pstr = str;
	
 	if (!pstr) return NULL;

	//delete left space
 	for(;pstr;)
	{
	   if (*pstr == ' ')
	      pstr++; 
	   else
	      return((char *)pstr);	
	}

	return NULL;
}



