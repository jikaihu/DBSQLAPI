#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char* get_version()
{
	static char version[256];

	sprintf(version, "dbsql version %s (built %s %s)\n", SYS_VERSION, __DATE__, __TIME__);

	return  version;
}

#ifdef __cplusplus
}
#endif



