#ifndef H_ORACLE8_H
#define H_ORACLE8_H


extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <oci.h>
//#include "ociapr.h"	//temp add in 20080209
}

#include "dbpub.h"
#include  "dbcomm.h"
#include  "dbsql.h"
#include  "dbapi.h"

#define DATA_TYPE_VARNUMBER     6  //var number  for long long
#define DATA_TYPE_LONG			8  // long column
#define DATA_TYPE_BINARY		23 // raw column
#define DATA_TYPE_LONGBINARY	24 // long raw column

#define SELECT_MODE				0
#define NON_SELECT_MODE			1

#define NO		0
#define YES		1

typedef char *LongType;


/*
enum DataType
{
    SQL_UNKNOWN = 0,
    SQL_INT,
    SQL_LONG,
    SQL_FLOAT,
    SQL_DATE,
    SQL_STRING,
    SQL_BINARY
};
*/


typedef struct {
	ub2		dbSize;
	sb2		dbType;
	sb1		buf[256];
	sb4		bufLen;
	sb4		dSize;
	sb2		precision;
	sb2		scale;
	sb2		nullOK;
} _describe;

typedef struct {
	ub1		*buf;
	double	fltBuf;
	int		intBuf;
    OCINumber numBuf;
	sb2		indp;
	ub2		colRetLen;
	ub2		colRetCode;
} _define;

typedef struct{
        DataType     type;
        int                 len;
        void              *data;
}_fld_val_t;


//-------------------------------------------------------------------
//-------------------------------------------------------------------
class  db_connect
{
    friend class db_cursor;
    friend class oracleapi;
private:

	  OCIEnv		*envhp;//environment handle
	  OCISvcCtx		*svchp;//service handle
	  OCIError		*errhp;//error handle
	  OCIServer		*srvhp;//server handle
	  OCISession	*authp;//authanication handle
	  ub4			init_mode;//initalize mode
	  ub4			credt; 
      OraText          msgbuf[512];

    enum conn_state
    {
      not_connected,
      connected
    };

	//连接状态
   conn_state m_state;

	int		  m_transaction;

	int initHandle();
	void  freeHandle();
	
	int attachServer(ub4 mode,OCIServer *srvhp,OCIError *errhp,OCISvcCtx *svchp,char *hoststring);
	void deatchServer();

public:
    db_connect();
    ~db_connect();
	
    int connect(char *user, char *password, char *hoststring);

    int disconnect();
    //当网络断或数据库down掉 , 重设 connect m_state=not_connected
    void  setNoConnct(void);     
    int isConnect(void);

    void  beginTrans();

    void  commitTrans();

    void  rollbackTrans();
    //获取oracle system error
    int   getErr();
    char* getErrInfo();
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
class   db_cursor
{
	friend class db_cursorList; 

private:
    	OCIStmt		*m_ociStmtp;
        OCIBind     *bndhp;
	db_connect	*m_connect;

	int		m_itemNum;	//select-list's item

	int			m_handle;
	db_cursor		*m_prev;
	db_cursor		*m_next;

	enum cursor_state
	{
		not_opened,
		opened
	};

	cursor_state m_state;

	_describe	m_describe[150];
	_define		m_define[150];

	//implicit describe select-list
	int implicitDefine();

public:
    db_cursor(); 
  	~db_cursor();
	
	//return record count that current transaction affect records
	int   affectRec() const;

    //open a cursor for execute sql statement
	int openCur(db_connect *conn_param);
	//close current cursor
  	int closeCur();
	//Bind an input variable
	int bindPos(int position,ub1 *progvar,int progvarlen,int datatype,int scale,sb2 *indicator);
  
//Bind an output variable

    int define(OCIDefine	*defnp, 
				 int  position, 
				 ub1	*buf, 
				 int	bufl, 
				 int	datatype, 
				 sb2	*indicator, 
				 ub2	*rlen, 
				 ub2	*rcode,
				 ub4	mode=OCI_DEFAULT);
 
	int define();
	
	//iMode		:	SELECT_MODE			execute select mode
	//			:	NON_SELECT_MODE		execute insert, update, delete mode
   int execute(int iMode=SELECT_MODE);

  	int fetch();

	char *fetchDefineFld();

        int output(TFieldList& fld_list);

	int outputStru(TFieldList& fld_list);

	void initDef();

	void freeDef();

	//prepare statement to execute
	int prepare(const char *pcSQLStmt);
};

//----------------------------------------------------------------
//----------------------------------------------------------------
class  db_cursorList {
	db_cursor	*pHead;
	db_cursor	*pTail;

public:
        db_cursorList();
        ~db_cursorList();
    
	void   init();

	db_cursor *find(int handle);

	int	   add(int handle);

	int	   del(int handle);

         void  free();
};


//----------------------------------------------------------------
//----------------------------------------------------------------
class  oracleapi:public dbapi
{
private:
    db_connect		m_connect;
    db_cursor		m_cursor;
    db_cursorList	m_curlist;


    OCINumber           m_ocinum[32];
    int                           m_numused;


    char                        m_username[64];
    char                        m_passwd[128];
    char                        m_hostname[64];
    char                        m_sql[MAX_DATA_BUF_LENGTH];
    _fld_val_t              m_bin_fld_val_list[32];

public:

    void phfree();

    oracleapi();

    ~oracleapi();

    int getErr();

    char* getErrInfo();
    
    int connect( char *user,char *password,char *hoststring );
    
    int checkConStatus(int reconInterval, int maxRetryTimes);
	
    int disconnect();

    inline int   changeDB(const char* db_name)
    	{	return 0;		}
	
    int getRec(const char *sql,TFieldList& fld_list);	

	//update a record , include a long column
	//longType = DATA_TYPE_LONG , DATA_TYPE_LONG_RAW ...
	// sql format : update tableName set col1=:1,col2='..' 
	//   now only support a represent bit symbol
private:
    int inSetSql(const char *sql);
    
    int inExec(bool isCommit);


public:
    int phSetSql(const char *sql);

    int phSetInt(unsigned int n, int *value);

    int phSetLonglong(unsigned int n, long long *value);

    int phSetString(unsigned int n, char *value,int len=0);

    int phSetLongbyte(unsigned int n, void *value, int len);

    int phExecute(bool isCommit=true);
	int setLongFld(const char *sql,char *longMsg,int longType,bool isCommit=true);

	bool isTable(const char *tab_name);

	int getSysDate(char *pcSysdate);

	bool checkSql(const char *pcSQLStmt, bool isSelectSQL);

	int executeSql(const char *sql, bool isCommit=true);

	//*iReturn	:	keep really record number that being affected
	int executeSql(const char *sql,int* iReturn, bool isCommit=true);

	void beginTrans();

	void commitTrans();

	void rollbackTrans();

	int query(const char *sql,int& handle);

	int fetch(int handle,TFieldList& fld_list);

	int noValueFetch(int handle);

	int closequery(int handle);

	int getCurRec(int handle,int *rowCount);

	int setDateFormat(DateFormat dateFormat);
//protected:
	inline void failover_transaction_cleanup()
	{	if(getErr() == 25402) rollbackTrans();		}

};
#endif


