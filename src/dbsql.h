#ifndef H_DBSQL_H
#define H_DBSQL_H

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
}

#include  "dbcomm.h"

#define MAX_ERR_MSG 1024
#define MAX_FIELD_NAME_LEN 32

enum  DB_Client
{
	Unknown,
	Oracle_Client,
	Mysql_Client,
	Redis_Client,
	Memcached_Client,
};


//value struct , using for database.dll
class TValue {
public:
    TValue();
    TValue(const TValue& other);
    TValue &operator = (const TValue& other);
    ~TValue();
    int asInt();
    unsigned int asUInt();
    long long asLLong();
    unsigned long long asULong();
    const char* asString();
    double asDouble();
    float asFloat();
    const char* asDate();
    const void* asBinary();
    const char* toString(char* sBuf = NULL);
    int getLength();
    DataType getType();
    bool isNULL();
    void setInt(int iValue);
    void setUInt(unsigned int iValue);
    void setFloat(float fValue);
    void setDouble(double dValue);
    void setLLong(long long lValue);
    void setULong(unsigned long long lValue);    
    void setDate(const char* sDate);        
    void setString(const char* sStr);        
    void setBinary(const char* sData, int iLen);        
    void setValue(const char* sData, int iLen);        
    void setNULL();
private:
    void bufferRead(const char* sData, int iLen, bool isString = false);
    void freeBuffer();
private:
    DataType eDataType;
    union uValueDefine
    {
        int i;
        unsigned int ui;
        float f;
        long long l;
        unsigned long long ul;
        double d;
        char *p;
    } uValue;
    int	 valueLen;    
};


class TField {
private:
    char fieldName[MAX_FIELD_NAME_LEN];
    TValue value;
    int op;
    int iSourceType;
public:
    TField();
    ~TField();
    void setFieldName(const char* sName);
    void setSourceType(int iType);
    void setFieldValue(TValue& value);
    void setOp(int iOp);
    TValue& fieldValue();
    int getOp();
    const char* getFieldName();
    int getSourceType();
private:
    TField(const TField& other);
    TField &operator = (const TField& other);
}; 


class TFieldList
{
private:    
    unsigned int num;
    TField *fld_list;

public:    
    TFieldList();
    ~TFieldList();
    void newFldList(int iFieldNum);
    void freeFldList();
    void printFldList();
    TField &operator[](unsigned int nField);
    TField &operator[](const char* sFieldName);
private:
    TFieldList(const TFieldList& other);
    TFieldList &operator = (const TFieldList& other);
};

class TException
{
public:
    TException();
    TException(const TException &other);
    TException(int iErrNo, const char* sErrMsg, ...);
    TException(int iNativeErrNo, int iErrNo, const char* sErrMsg, ...);
    virtual ~TException();
    int getErrNo();
    int getNativeErrNo();
    const char* getErrInfo();
protected:
    ErrorType sql_error_type;
    int sql_err_no;
    int native_err_no;
    char sql_err_info[MAX_ERR_MSG];
};

class dbapi;
class  dbsql {
private:
    DB_Client used_db;
    dbapi * pdb;
public:
    dbsql();
    dbsql(const char * db);
    dbsql(DB_Client edb);
    ~dbsql();    
    void initDB(DB_Client edb);
    inline DB_Client getDBSource()
    	{	return used_db;	}
	
    int getErr();
    char* getErrInfo();
    void connect( char *user,char *password,char *hoststring );
    int checkConStatus(int reconInterval, int maxRetryTimes);
    void disconnect();		
    int getRec(const char *sql, TFieldList& fld_list);	
    void phfree();
    void phSetSql(const char *sql);
    void phSetInt(unsigned int n, int *value);
    void phSetLonglong(unsigned int n, long long *value);
    void phSetString(unsigned int n, char *value,int len=0);
    void phSetLongbyte(unsigned int n, void *value, int len);
    void phExecute(bool isCommit=true);
    void setLongFld(const char *sql,char *longMsg,int longType,bool isCommit=true);
    bool isTable(const char *tab_name);
    void getSysDate(char *pcSysdate);
    bool checkSql(const char *pcSQLStmt, bool isSelectSQL);
    void executeSql(const char *sql, bool isCommit=true);
    void executeSql(const char *sql,int *iReturn, bool isCommit=true);
    void beginTrans();
    void commitTrans();
    void rollbackTrans();
    void query(const char *sql, int& handle);
    int fetch(int handle, TFieldList& fld_list);
    void noValueFetch(int handle);
    void closeQuery(int handle);
    void getCurRec(int handle,int *rowCount);
    void setDateFormat(DateFormat dateFormat);
    void   changeDB(const char* db_name);
};


class TGlobals
{
public:
    static const char* getVersion();
};


#endif


