#ifndef _DBHANDLE_H
#define _DBHANDLE_H 

#include <pthread.h>

class dbHandle{
public:
    dbHandle();
    virtual ~dbHandle() ;
    void loadLibrary();

protected:    
    virtual void loadDBFunc(void* dlHandle) = 0;
    void loadFunc(void* dlHandle, void*& pFunc, const char* pMethod);
    
protected:
    const char* pLibraryName;
    bool bLoadHandleFlag;
    pthread_mutex_t loadLibMutex;
};


#endif

