#include "dbapi.h"
#include "dbsql.h"



dbapi::dbapi(){}
 dbapi::~dbapi(){}

int dbapi::getErr(){}
void dbapi::phfree(){}

char* dbapi::getErrInfo(){}

int dbapi::connect( char *user,char *password,char *hoststring ){}

int dbapi::checkConStatus(int reconInterval, int maxRetryTimes){}

int dbapi::disconnect(){}

int   dbapi::changeDB(const char* db_name){}

int dbapi::getRec(const char *sql,TFieldList& fld_list){}	

int dbapi::phSetSql(const char *sql){}

int dbapi::phSetInt(unsigned int n, int *value){}

int dbapi::phSetLonglong(unsigned int n, long long *value){}

int dbapi::phSetString(unsigned int n, char *value,int len){}

int dbapi::phSetLongbyte(unsigned int n, void *value, int len){}

int dbapi::phExecute(bool isCommit){}

int dbapi::setLongFld(const char *sql,char *longMsg,int longType,bool isCommit){}

bool dbapi::isTable(const char *tab_name){}

int dbapi::getSysDate(char *pcSysdate){}

bool dbapi::checkSql(const char *pcSQLStmt, bool isSelectSQL){}

int dbapi::executeSql(const char *sql, bool isCommit){}

int dbapi::executeSql(const char *sql,int* iReturn, bool isCommit){}

void dbapi::beginTrans(){}

void dbapi::commitTrans(){}

void dbapi::rollbackTrans(){}

int dbapi::query(const char *sql,int& handle){}

int dbapi::fetch(int handle,TFieldList& fld_list){}

int dbapi::noValueFetch(int handle){}

int dbapi::closeQuery(int handle){}

int dbapi::getCurRec(int handle,int *rowCount){}

int dbapi::setDateFormat(DateFormat dateFormat){}


