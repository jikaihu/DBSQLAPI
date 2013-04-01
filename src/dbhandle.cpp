#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "dbhandle.h"
#include "dbsql.h"
#include "dbsqlapierror.h"

dbHandle::dbHandle() 
{
    pLibraryName = NULL;
    bLoadHandleFlag = false;
    pthread_mutex_init(&loadLibMutex, NULL);
}

dbHandle::~dbHandle()
{

}

    
void dbHandle::loadLibrary()
{
    if(!bLoadHandleFlag)
    {
        pthread_mutex_lock(&loadLibMutex);
        void *dl_handle = NULL;
        char *error = NULL;

        /* Open the shared object */
        dl_handle = dlopen(pLibraryName, RTLD_GLOBAL | RTLD_NOW);
        if (!dl_handle) 
        {
            pthread_mutex_unlock(&loadLibMutex);
            throw TException(LIB_ERR_DLOPEN_ERROR,  LIB_ERR_INFO_DLOPEN_ERROR, dlerror() );
        }

        loadDBFunc(dl_handle);
        
        /* Close the object */
        //dlclose( dl_handle );
        bLoadHandleFlag = true;
        pthread_mutex_unlock(&loadLibMutex);
    }
}

void dbHandle::loadFunc(void* dlHandle, void*& pFunc, const char* pMethod)
{
    char* error = NULL;
    pFunc = dlsym( dlHandle, pMethod );
    error = dlerror();
    if (error != NULL)\
    {
        throw TException(LIB_ERR_DLSYM_ERROR,  LIB_ERR_INFO_DLSYM_ERROR, pMethod, error);
    }
    return;
}

    
