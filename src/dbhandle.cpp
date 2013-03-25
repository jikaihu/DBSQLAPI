#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "dbhandle.h"
#include "dbsql.h"

dbHandle::dbHandle() 
{
    pLibraryName = NULL;
    bLoadHandleFlag = false;
}

dbHandle::~dbHandle()
{

}

    
void dbHandle::loadLibrary()
{
    if(bLoadHandleFlag)
    {
        void *dl_handle = NULL;
        char *error = NULL;

        /* Open the shared object */
        dl_handle = dlopen(pLibraryName, RTLD_LAZY );
        if (!dl_handle) 
        {
            throw TException(9999,  "!!! %s\n", dlerror() );
        }

        loadDBFunc(dl_handle);
        
        /* Close the object */
        dlclose( dl_handle );
        bLoadHandleFlag = true;
    }
}

void dbHandle::loadFunc(void* dlHandle, void* pFunc, const char* pMethod)
{
    char* error = NULL;
    pFunc = dlsym( dlHandle, pMethod );
    error = dlerror();
    if (error != NULL)\
    {
        throw TException(9999, "!!! %s\n", error );
    }
    return;
}

    
