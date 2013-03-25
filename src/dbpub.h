#ifndef _H_DBPUB_H
#define _H_DBPUB_H

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
}


#define MAXITEMS	150	  /* maximum select-list items               */
/*
class TDBMsg {
public:    
    unsigned int sequence;
    unsigned int tabid;
    unsigned int op;
    TField condition;
    TFieldList rec;
};
*/

class TSTRUTILS{
public:    
    static char *str_dup(const char *src);
    static void free_dup(char *&src);
    static char *ltrim(char *str);
    static void rtrim(char *str);
    static char	*trim(char *str);
};

#endif

