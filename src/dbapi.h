#ifndef H_DBAPI_H
#define H_DBAPI_H

class dbapi
{
public:
    dbapi(){};
    virtual ~dbapi(){};
    virtual int getErr();
    virtual void phfree();

    virtual char* getErrInfo();
    
    virtual int connect( char *user,char *password,char *hoststring );
    
    virtual int checkConStatus(int reconInterval, int maxRetryTimes);
	
    virtual int disconnect();

    virtual int   changeDB(const char* db_name);
	
    virtual int getRec(const char *sql,TFieldList& fld_list);	

    virtual int phSetSql(const char *sql);

    virtual int phSetInt(unsigned int n, int *value);

    virtual int phSetLonglong(unsigned int n, long long *value);

    virtual int phSetString(unsigned int n, char *value,int len=0);

    virtual int phSetLongbyte(unsigned int n, void *value, int len);

    virtual int phExecute(bool isCommit=true);
    virtual int setLongFld(const char *sql,char *longMsg,int longType,bool isCommit=true);

    virtual bool isTable(const char *tab_name);

	virtual int getSysDate(char *pcSysdate);

	virtual bool checkSql(const char *pcSQLStmt, bool isSelectSQL);

	virtual int executeSql(const char *sql, bool isCommit=true);

	virtual int executeSql(const char *sql,int* iReturn, bool isCommit=true);

	virtual void beginTrans();

	virtual void commitTrans();

	virtual void rollbackTrans();

	virtual int query(const char *sql,int& handle);

	virtual int fetch(int handle,TFieldList& fld_list);

	virtual int noValueFetch(int handle);

	virtual int closequery(int handle);

	virtual int getCurRec(int handle,int *rowCount);

	virtual int setDateFormat(DateFormat dateFormat);
};

#endif
