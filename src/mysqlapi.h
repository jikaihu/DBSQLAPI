#ifndef H_MYSQLAPI_H
#define H_MYSQLAPI_H

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <mysql.h>
#include <mysqld_error.h>
//#include <mysqld_ername.h>
#include <errmsg.h>
}
#include <dbpub.h>
#include  "dbcomm.h"
#include "dbsql.h"
#include "dbapi.h"

/*

enum MysqlDataType
{
    MYSQL_UNKNOWN = 0,
    MYSQL_INT,
    MYSQL_LONG,
    MYSQL_FLOAT,
    MYSQL_DATE,
    MYSQL_STRING,
    MYSQL_BINARY,
    MYSQL_TEXT
};

*/

typedef struct{
        DataType     type;
        int               len;
	 unsigned long	data_len;
        void             *data;
}mysql_fld_val_t;


extern "C" {

#define STRING_SIZE 64

typedef struct {
	char			col_name[STRING_SIZE];
	void*		buf;
	unsigned long length;
	unsigned long max_length;
  enum_field_types type;
	my_bool       is_null;
	my_bool       error;
} mysql_define;
}


const int BUFFER_ALL_RESULTS_TO_CLIENTSIDE = 1;



const int MAX_ENV_STRING_LENGTH = 64,MAX_MSG_BUFFER_LENGTH = 512;
const int MYSQL_SELECT_MODE = 0,MYSQL_NON_SELECT_MODE = 1;
const int MY_YES = 1,MY_NO = 0;
const char db[] = "hitv";

enum my_sql_conn_state
  {
      not_connected,
      connected
  };

class  mysql_connect
{
    friend class mysql_cursor;
    friend class mysqlapi;
private:  
    MYSQL   		mysql; 
	 
    char    msgbuf[MAX_MSG_BUFFER_LENGTH];

    

   my_sql_conn_state db_state;

	int		  transaction;

public:
    mysql_connect();
    ~mysql_connect();
	
    int connect(const char *user, 
    			     const char *password, 
    			     const char *host = "localhost",
    			     const char *database = db,
    			     unsigned long client_flag = CLIENT_MULTI_STATEMENTS ||CLIENT_FOUND_ROWS,
    			     unsigned int port = 3306,//3307
    			     const char * unix_socket = "/tmp/mysql.sock");

    int    		disconnect();
    
    void  		setNoConnct(void);     
    int    		isConnect(void);
    
    void  		beginTrans();
    void  		commitTrans();
    void  		rollbackTrans();
    
    unsigned int   getErr();
    char* 		getErrInfo();
    
    int			changeDB(const char*);
    int			flashDB();
    int   			processDdl(const char*);
    static      void    mysqlEnvInit();
};

template <class Type>  class ListNode 
{
	public:
	Type data;
	int handle;
	ListNode<Type> *prev,*next;
	ListNode() : data(Type()), handle(0),prev(NULL),next(NULL) {} 
	ListNode(const Type &item) : data(item), handle(0),prev(NULL),next(NULL) {}
	ListNode(const Type &item, int p) : data(item), handle(p),prev(NULL),next(NULL) {}
};


template <class Type>  class  mysql_cursorList 
{
	ListNode<Type>	*pHead;
	ListNode<Type>	*pTail;
	int index ;	//not thread safe  
public:
        mysql_cursorList() : pHead(NULL),pTail(NULL),index(0) {}
        ~mysql_cursorList()
        {
        	free();
        }

				ListNode<Type> * _find(int s_handle)
				{
					ListNode<Type> *ptr = pHead;			
					for(;ptr && ptr->handle!=s_handle;)
						ptr = ptr->next;
					if(ptr)
						return ptr;
					else
						return NULL;
				}
				
				Type * find(int s_handle)
				{
					ListNode<Type> * ptr = _find(s_handle);
					if (ptr)
							return (&(ptr->data));
					else
							return NULL;
				}
				
				int	   add(int i_handle)
				{
					ListNode<Type> *ptr = new ListNode<Type>;
			
					if(!pHead)
					{
						pHead = ptr;
						ptr->prev = NULL;
					}
					else
					{
						pTail->next = ptr;
						ptr->prev = pTail;
					}
				
					ptr->next = NULL;
					pTail = ptr;
				
					ptr->handle = ++index;
					//ptr->handle = i_handle;
					
					return ptr->handle;
				}
			
				int	   del(int d_handle)
				{
					ListNode<Type> *ptr;

					if((ptr=_find(d_handle))==NULL)
						return(1);
					
					if(ptr==pHead) 
						pHead = pHead->next;
					else
						ptr->prev->next = ptr->next;
				
					if(ptr==pTail)
						pTail = pTail->prev;
					else
						ptr->next->prev = ptr->prev;
				
					delete ptr;
				
					return 0;
				}
			
			  void  free()
			  {
			  	ListNode<Type>	*curptr = pHead;
			    ListNode<Type>	*nextptr = NULL;
			
			    for(;curptr; curptr = nextptr)
			    {
			         nextptr = curptr->next;
			         del(curptr->handle);
			    }
			    pHead=pTail=NULL;
			  }
};


class   mysql_cursor
{
private:
	mysql_connect*               m_connect;
	
       MYSQL_STMT*                  stmtp;
       MYSQL_RES*                    prepare_meta_result;
    
       
       MYSQL_BIND*                  parm_bind;
       MYSQL_BIND*                  mybind;
       mysql_define*                  mydefine;
       static DateFormat		    m_dateformat;
     
   	int		               m_itemNum;	//select-list's item
   	int				  m_row_count;
	
	char    msgbuf[MAX_MSG_BUFFER_LENGTH];
	unsigned int stmt_err_no;
	enum cursor_state
	{
		not_opened,
		opened
	};

	cursor_state m_state;

	
	int _getParamCount();
	int _getDefineColCount();
	int _getDefineMetaInfo();
	int _getBindMetaInfo();

public:
      mysql_cursor(); 
  	~mysql_cursor();
	
	//return record count that current transaction affect records
	int   affectRec() const;

      //open a cursor for execute sql statement
	int openCur(mysql_connect *conn_param);
	//close current cursor
  	int closeCur();
	//Bind an input variable
	int    allocateParmBindStruct();
	void  releaseParmBindStruct();
	int    bindPos(int position,
       	                    char *in_param_buf,
       	                    enum enum_field_types  in_datatype,
       	                    unsigned long in_buffer_length,
       	                    unsigned long * in_length=NULL,
       	                    my_bool * in_is_null = 0,
       	                    my_bool  in_is_unsigned = true);
	int bindLargeParm(int position,const char *data, unsigned long length);
       int bindParm();
    
    
      //Bind an output variable
	int allocateDefineStruct();

	void releaseDefineStruct();  
 
	int define();
	
	
	int prepare(const char *sql_str);
	//iMode		:	SELECT_MODE			execute select mode
	//			:	NON_SELECT_MODE		execute insert, update, delete mode
      int execute(int iMode=MYSQL_SELECT_MODE);
      int getResultsRowCount();
      int fetch();

	unsigned int   getStmtErrNo();
    	char* 	     getStmtErrInfo();
	void 		     reportErrorInfo(unsigned int * errno,char * perrbuf);
	bool			isStmtResultNoData(int result);

	static inline void setDateFormat(DateFormat df)
		{	m_dateformat = df;				}
	
	inline enum enum_field_types get_datatype_TEXT_id()
		{	return MYSQL_TYPE_STRING;	}
	inline enum enum_field_types get_datatype_BLOB_id()
		{	return MYSQL_TYPE_BLOB;		}
	
	char *fetchBlobColumn(unsigned int col_pos);

       int output(TFieldList& fld_list);

	int outputStru(TFieldList& fld_list);
	
};




class  mysqlapi:public dbapi
{
private:
    mysql_connect		m_connect;
    mysql_cursor		m_cursor;
    mysql_cursorList<mysql_cursor>	m_curlist;

    char                        m_username[64];
    char                        m_passwd[128];
    char                        m_hostname[64];
    char                        m_sql[MAX_DATA_BUF_LENGTH];
    mysql_fld_val_t                m_bin_fld_val_list[32];
    unsigned long 		m_bin_data_parm_len;
    enum operation
    {	
    	system,
	statement,
	dynamic_statement
    };
    operation now_operation;
    char    d_stat_msgbuf[MAX_MSG_BUFFER_LENGTH];
    unsigned int d_stmt_err_no;
    TFieldList fldlist;

public:

    void phfree();

    mysqlapi();

    ~mysqlapi();

    int getErr();

    char* getErrInfo();
    
    inline int getStmtErrNo()
    	{	return m_cursor.getStmtErrNo();	}

    inline char* getStmtErrInfo()
    	{	return m_cursor.getStmtErrInfo();	}
   	
    int connect( char *user,char *password,char *hoststring = "localhost");
    
    int checkConStatus(int reconInterval, int maxRetryTimes);
	
    int disconnect();					
	
    int getRec(const char *sql,TFieldList& fld_list);	


private:
	
   int inSetSql(const char *sql);
    
   int inExec(bool isCommit);


public:

    int phSetSql(const char *sql);

    int  phsetParm(unsigned int n,DataType d_type,void* value,int len=0);

    inline int phSetInt(unsigned int n, int *value)
	{	return phsetParm(n-1,SQL_INT,value);}

    inline int phSetLonglong(unsigned int n, long long *value)
	{	return phsetParm(n-1,SQL_LONG,value);}
	
    inline int phSetString(unsigned int n, char *value,int len)
	{	return phsetParm(n-1,SQL_STRING,value,(len == 0) ? strlen(value) : len);}

    inline int phSetLongbyte(unsigned int n, void *value, int len)
	{	return phsetParm(n-1,SQL_BINARY,value,len);}	

	int phExecute(bool isCommit=true);
	

	bool isTable(const char *tab_name);

	int getSysDate(char *pcSysdate);

	bool checkSql(const char *pcSQLStmt, bool isSelectSQL=true);

	//*iReturn	:	keep really record number that being affected
	int executeSql(const char *sql,int *iReturn=NULL, bool isCommit=true);

	inline int executeSql(const char *sql, bool isCommit=true)
		{ return executeSql(sql,NULL,isCommit);	}

	int setLongFld(const char *sql,char *longMsg,unsigned long length,int longType,bool isCommit=true);

	int beginLoadBinaryData(const char* sql,int col_type);

	//It can be called multiple times to send the parts of a character or binary data value for a column, which must be one of the TEXT or BLOB data types. 
	int transferBinData(const char * pdata,unsigned long data_len);

	int endLoadBinaryData(bool is_commit =true);

	void beginTrans();

	void commitTrans();

	void rollbackTrans();

	int   changeDB(const char* db_name);

	int   processDdl(const char* ddl_sql);

	int query(const char *sql,int& handle);

	int fetch(int handle,TFieldList& fld_list);

	int noValueFetch(int handle);

	int closequery(int handle);

	int getCurRec(int handle,int* rowCount);

	int setDateFormat(DateFormat dateFormat);

};
#endif

