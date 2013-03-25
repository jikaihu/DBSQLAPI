extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <syslog.h>
}
#include <mysqlapi.h>

#include "mysqlhandle.h"
//#define DEBUG

mysqlHandle g_mysql;

extern "C" {
 
static void handle_sig_pipe(int sig)
{
        #ifdef DEBUG
                //fprintf(stderr,"Now ignore SIGPIPE .Sig:%d\n",sig);
                openlog("DBAPI--Mysql Driver:",LOG_ODELAY|LOG_PID|LOG_PERROR,LOG_USER);
                syslog(LOG_NOTICE,"Now ignore SIGPIPE .Sig:%d\n",sig);
                closelog();
        #endif
        return;
}
}
 
struct env_action
{
        env_action()
        {
                mysql_connect::mysqlEnvInit();
        }
};
 
//struct env_action ea_instance;

static int date_format(char* buf,int buf_len,DateFormat df,MYSQL_TIME* row_buf)
{
	if(!buf ||!row_buf)
		return -1;
	
	switch (df){
	case default_date :
		snprintf(buf,buf_len,"%04d%02d%02d%02d%02d%02d", 
						  row_buf->year,
				                    row_buf->month,
				                    row_buf->day,
				                    row_buf->hour,
				                    row_buf->minute,
				                    row_buf->second);
		break;
	case chinese_date:
		snprintf(buf,buf_len,"%04d-%02d-%02d", 
						  row_buf->year,
				                    row_buf->month,
				                    row_buf->day
				                    );
		break;
	case chinese_datetime:
		snprintf(buf,buf_len,"%04d-%02d-%02d %02d:%02d:%02d", 
						  row_buf->year,
				                    row_buf->month,
				                    row_buf->day,
				                    row_buf->hour,
				                    row_buf->minute,
				                    row_buf->second);
		break;
	default:
		snprintf(buf,buf_len,"%02d-%02d-%04d", 
						  row_buf->day,
				                    row_buf->month,
				                    row_buf->year
				                    );
		break;
	}
	
	return 0;
}

//*************************************************************************
//********************  mysql_connect  *********************************
//*************************************************************************
void    mysql_connect::mysqlEnvInit()
{
        #ifdef DEBUG
            fprintf(stderr,"Now calling mysqlEnvInit().\n");
        #endif
        
        if (g_mysql.mysql_library_init(0, NULL, NULL))
        {
                #ifdef DEBUG
                        fprintf(stderr, "could not initialize MySQL library\n");
                        exit(1);
                #endif
        }
        
        struct sigaction action,oaction;
        action.sa_handler = handle_sig_pipe;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        if(sigaction(SIGPIPE, &action, &oaction)<0)
        {
                #ifdef DEBUG
                        fprintf(stderr, "Install Sig Handle Error!\n");
                        exit(1);
                #endif
        }
        //raise(SIGPIPE);
        return;
}

mysql_connect::mysql_connect()
{
	db_state = not_connected;
         g_mysql.loadLibrary();
    
	g_mysql.mysql_init(&mysql);
	my_bool reconnect = 1; //0:disable 1:enable
	g_mysql.mysql_options(&mysql, MYSQL_OPT_RECONNECT, &reconnect);
	my_bool is_truncation = 0;
	g_mysql.mysql_options(&mysql,MYSQL_REPORT_DATA_TRUNCATION,&is_truncation);
	msgbuf[0]='\0';
}

mysql_connect::~mysql_connect()
{
	if(db_state == connected)
	{
		disconnect();
	}
}

int mysql_connect::connect(const char *user, 
					    	    const  char *password, 
					    	    const  char *host,
					    	    const  char *database,
					    	    unsigned long client_flag,
					    	    unsigned int port,
					    	    const char * unix_socket)
{
	if(db_state == connected)
	{
		return -1;
	}
	if(g_mysql.mysql_real_connect(&mysql,
						host,
						user,
						password,
						database,
						port,
						unix_socket,
						client_flag) == NULL)
	{
		return -1;
	}
	db_state = connected;
	transaction = MY_NO;
	return 0;
}

int mysql_connect::disconnect()
{
	if(db_state == not_connected)
	{
		return -1;
	}
	g_mysql.mysql_close(&mysql);
	db_state = not_connected;

      return 0;
}

void mysql_connect::setNoConnct(void)
{
	db_state = not_connected;
}

int mysql_connect:: isConnect(void)
{
    if(db_state == not_connected)
    {
        return -1;
    }
    return 0;
}

void mysql_connect::beginTrans()
{
	my_bool status;
	if(transaction) 
	    status = g_mysql.mysql_commit(&mysql);;
	transaction = MY_YES;
}


void mysql_connect::commitTrans()
{
	my_bool status = g_mysql.mysql_commit(&mysql);
	transaction = MY_NO;
	if(*g_mysql.mysql_error(&mysql))
	{
	    return ;
	}

}

void mysql_connect::rollbackTrans()
{
	my_bool status = mysql_rollback(&mysql);
	transaction = MY_NO;
	if(*g_mysql.mysql_error(&mysql))
	{
	     return ;
	}
}

unsigned int mysql_connect::getErr()
{
	unsigned int err_no;
	err_no = g_mysql.mysql_errno(&mysql);

	if(err_no == CR_SERVER_GONE_ERROR  ||
	   err_no  == CR_SERVER_LOST  || 
	   err_no == CR_CONN_HOST_ERROR ||
	   err_no == CR_CONNECTION_ERROR ||
	   err_no == CR_SERVER_LOST_EXTENDED ||
	   err_no == ER_SERVER_SHUTDOWN)	
		return -99999;
	
	return err_no;    
}

char* mysql_connect::getErrInfo()
{
    const char * _res =  g_mysql.mysql_error(&mysql);
    if(!_res[0])
	return NULL;
    
    if (strcmp(msgbuf,_res) == 0)
		return msgbuf;

    return (strncpy(msgbuf,_res,strlen(_res)));  
}

int		mysql_connect::changeDB(const char* dbstring)
{
	if (dbstring)
	{
		if(g_mysql.mysql_select_db(&mysql,dbstring) != 0)
			return -1;
	}
	else
	{
		return -1;
	}
	return 0;
}

int mysql_connect::flashDB()
{
	if(g_mysql.mysql_refresh(&mysql,REFRESH_GRANT | REFRESH_LOG | REFRESH_TABLES | REFRESH_HOSTS) != 0)
		return -1;
	return 0;
}

int mysql_connect::processDdl(const char* sql_str)
{
	if(g_mysql.mysql_real_query(&mysql,sql_str,strlen(sql_str)) != 0)
		return -1;
    return 0;
}

//*************************************************************************
//********************  mysql_cursor  *********************************
//*************************************************************************
DateFormat mysql_cursor::m_dateformat = default_date;

mysql_cursor::mysql_cursor() 
{   
	m_state = not_opened; 
	m_connect = (mysql_connect *)0; 
    
    stmtp = NULL;
    prepare_meta_result = NULL;
    
    parm_bind = NULL;
    mybind = NULL;
    mydefine = NULL;
    
    m_itemNum = 0;
    //m_param_num = 0;
    m_row_count = 0;
    
    stmt_err_no = 0;
    //msgbuf[0]='\0';
    memset(msgbuf,0,MAX_MSG_BUFFER_LENGTH);
    
}
 
mysql_cursor::~mysql_cursor()
{
    if(m_state == opened) 
	   closeCur();
    
    releaseParmBindStruct();
    releaseDefineStruct(); 
}

int mysql_cursor::openCur(mysql_connect *conn_param)
{
	if(m_state == opened)
	{
		return -1;
	}
	if(!conn_param || conn_param->db_state!= connected)
	{
		return -1;
	}
	
	if(!(stmtp = g_mysql.mysql_stmt_init(&(conn_param->mysql))))
	{
		m_state = not_opened;	
		m_connect = conn_param;
		return -1;
	}
	else
	{
		m_state = opened;
		m_connect = conn_param;  
	}
	return 0;
}


int mysql_cursor::closeCur()
{
    int status=-1;
    my_bool ret;
    if(m_state == not_opened)
    {
	 return -1;
    }
	
    if(prepare_meta_result)
    {
	 g_mysql.mysql_free_result(prepare_meta_result);
	 prepare_meta_result = NULL;
    }
	
    if(stmtp)
    {
        ret = g_mysql.mysql_stmt_close(stmtp);
        stmtp = NULL;
    }

    m_state = not_opened;
    m_connect = (mysql_connect *)0;
    m_row_count = 0;
    if(0 == ret )
    	status = 0;
    
    return (status);
}

//***************************************DEFINE FUNC SET **************************************************************
int mysql_cursor::_getDefineColCount()
{
	if(prepare_meta_result)
	{
		g_mysql.mysql_free_result(prepare_meta_result);
		prepare_meta_result = NULL;
	}
	prepare_meta_result = g_mysql.mysql_stmt_result_metadata(stmtp);
	if (!prepare_meta_result)
		return -1;
	
	m_itemNum = g_mysql.mysql_num_fields(prepare_meta_result);	
	if(m_itemNum <=0)
		return -1;
	
	return 0;
}


int mysql_cursor::_getDefineMetaInfo()
{
	MYSQL_FIELD *field;
	int i = 0;
	
	if(!prepare_meta_result)
		return -1;
	
	while((field = g_mysql.mysql_fetch_field(prepare_meta_result)))
	{
	    #ifdef DEBUG
	    fprintf(stderr,"column[%d]:\n\tname:%s\n\torg_name:%s\n\tlength:%d\n\tmax_length:%d\n\tenum_field_types:%d\n",
	            i,field->name,field->org_name,field->length,field->max_length,field->type);
	    #endif
	    
	    strcpy(((mysql_define*)(mydefine+i))->col_name,field->name);
	    ((mysql_define*)(mydefine+i))->type =  field->type;
	    ((mysql_define*)(mydefine+i))->length = field->length;//field->max_length;
	    
	     if(field->flags & UNSIGNED_FLAG)
	    		(mybind+i)->is_unsigned=true;
	     else
			(mybind+i)->is_unsigned=false;
	    i++;
	}
	
	return 0;
}

int mysql_cursor::allocateDefineStruct()
{
    releaseDefineStruct();
	
    if(_getDefineColCount()<0)
		return -1;
		
    if((mydefine = new mysql_define[m_itemNum]) == NULL)
		return -1;
    memset(mydefine,0,sizeof(mysql_define)*m_itemNum);
        
    if((mybind = new MYSQL_BIND[m_itemNum]) == NULL)
    	return -1;
    memset(mybind,0,sizeof(MYSQL_BIND)*m_itemNum);
	
    if(_getDefineMetaInfo()<0)
		return -1;
		
    return 0;
}

//Call this FUNC to release resource before fetch result set meta information
void mysql_cursor::releaseDefineStruct()
{
	if (mydefine && m_itemNum >0)
	{
	   	for(int i = 0;i<m_itemNum;i++)
	   	   if(((mysql_define*)(mydefine+i))->buf)
	   	   	  free((mydefine+i)->buf);
	   	
	   	delete [] mydefine;
	   	mydefine = NULL;    
	}
	
	
	if (mybind && m_itemNum >0)
	{
		delete [] mybind;
		mybind = NULL;
	}
	
	m_itemNum = 0;
	
	return;
}


int mysql_cursor::define()
{
	if(allocateDefineStruct()<0)
		return -1;
	int i;	
	for(i=0;i<m_itemNum;i++)
	{
	    (mybind+i)->buffer_type= (mydefine+i)->type;	    
	    (mybind+i)->is_null= &(((mydefine+i))->is_null);
	    (mybind+i)->length= &(((mydefine+i))->length);
	    (mybind+i)->error= &(((mydefine+i))->error);
	   

	    if((mydefine+i)->type == MYSQL_TYPE_DATETIME)
	    {
		(mydefine+i)->buf = malloc(sizeof(MYSQL_TIME)+1);
		(mybind+i)->buffer_length = sizeof(MYSQL_TIME);
	    }
	    else
	    {
	    	(mydefine+i)->buf = malloc((mydefine+i)->length+1);
		(mybind+i)->buffer_length = ((mydefine+i))->length;
	    }
	    memset((mydefine+i)->buf,0,(mydefine+i)->length+1);
	    (mybind+i)->buffer= (mydefine+i)->buf;
	    
	     #ifdef DEBUG
	       fprintf(stderr,"bind[%d]:\n\ttype:%d\n\tcol_name:%s\n\tmalloc_buf_len:%d\n",
	                i,((mybind+i))->buffer_type,((mydefine+i))->col_name,(mydefine+i)->length+1);
		//fprintf(stderr,"sizeof(MYSQL_TIME) = %d\n",sizeof(MYSQL_TIME));
	     #endif
	}
	
	if (g_mysql.mysql_stmt_bind_result(stmtp, mybind) != 0)
		return -1;
	#ifdef DEBUG	
	for(i=0;i<m_itemNum;i++)
	{
		 fprintf(stderr,"bind[%d]:\n\ttype:%d\n\tcol_name:%s\n\tmalloc_buf_len:%d\n\tis_unsigned:%d\n",
	                i,((mybind+i))->buffer_type,((mydefine+i))->col_name,(mydefine+i)->length+1,(mybind+i)->is_unsigned );
	}
	#endif
    return 0;
    
}


//****************************************************************************************************


//*********************************** BIND FUNC SET**********************************************
int mysql_cursor::_getParamCount()
{
	if (stmtp)
		return g_mysql.mysql_stmt_param_count(stmtp);
      else
    	      return -1;
}

int mysql_cursor::_getBindMetaInfo()
{
	MYSQL_FIELD *field;
	int i = 0;
     
	prepare_meta_result = g_mysql.mysql_stmt_param_metadata(stmtp) ;
	
	if(!prepare_meta_result)
	{
		printf("mysql_stmt_param_metadata\n");
		return -1;
	}
	while((field = g_mysql.mysql_fetch_field(prepare_meta_result)))
	{    
	     if(field->flags & UNSIGNED_FLAG)
	    	(mybind+i)->is_unsigned=true;
	     else
		(mybind+i)->is_unsigned=false;
	    #ifdef DEBUG
	    fprintf(stderr,"_getBindMetaInfo\ncolumn[%d]:\n\tname:%s\n\torg_name:%s\n\tlength:%d\n\tmax_length:%d\n\tenum_field_types:%d\nfield->flags:%d\n(mybind+i)->is_unsigned:%d\n",
	            i,field->name,field->org_name,field->length,field->max_length,field->type,field->flags,(mybind+i)->is_unsigned);
	    #endif 
	    i++;
	}
	
	return 0;
}

int mysql_cursor::allocateParmBindStruct()
{
    int parm_count = 0;
    if((parm_count = _getParamCount()) <=0)
	  return -1;
	
    releaseParmBindStruct();
	
    parm_bind = new MYSQL_BIND[parm_count];
    memset(parm_bind,0,sizeof(MYSQL_BIND)*parm_count);
    
    //_getBindMetaInfo();
    return 0;
}


void mysql_cursor::releaseParmBindStruct()
{
	if(parm_bind)
	{
		delete [] parm_bind;
		parm_bind = NULL;
	}
	
	return;
}

int mysql_cursor::bindPos(int position,
	                                     char *in_param_buf,
	                                     enum enum_field_types  in_datatype,
	                                     unsigned long in_buffer_length,
	                                     unsigned long * in_length,
	                                     my_bool * in_is_null,
	                                     my_bool in_is_unsigned)
{
	if(position >= 0 && parm_bind)
	{
		parm_bind[position].buffer_type = in_datatype;
		parm_bind[position].buffer = in_param_buf;
		parm_bind[position].buffer_length = in_buffer_length;
		parm_bind[position].length = in_length;
		parm_bind[position].is_null = in_is_null;
		parm_bind[position].is_unsigned = in_is_unsigned;
		
	}
	
	return 0;
}

int mysql_cursor::bindLargeParm(int position,const char *data, unsigned long length)
{
	return g_mysql.mysql_stmt_send_long_data(stmtp,position,data,length);
}

int mysql_cursor::bindParm()
{
	return g_mysql.mysql_stmt_bind_param(stmtp,parm_bind);
}

//***************************************************************************************************************
unsigned int   mysql_cursor::getStmtErrNo()
{
	if(stmtp)
		return (stmt_err_no=g_mysql.mysql_stmt_errno(stmtp));
	else
		return stmt_err_no;
}

char* mysql_cursor::getStmtErrInfo()
{
	if(stmtp && this->getStmtErrNo()>0)
	{
		const char* ret = g_mysql.mysql_stmt_error(stmtp);
    		strncpy(msgbuf,ret,strlen(ret));  
	}
	return msgbuf;
}

void 	mysql_cursor::reportErrorInfo(unsigned int * errno,char * perrbuf)
{
	*errno = this->getStmtErrNo();
	strncpy(perrbuf,this->getStmtErrInfo(),strlen(this->getStmtErrInfo()));
}


bool	 mysql_cursor::isStmtResultNoData(int result)
{
	if(result == MYSQL_NO_DATA)
	{
		this->stmt_err_no = MYSQL_NO_DATA;
		strcpy(msgbuf,"No more rows/data exists!");
		return true;
	}
	else if(result == MYSQL_DATA_TRUNCATED)
	{
		this->stmt_err_no = MYSQL_DATA_TRUNCATED;
		strcpy(msgbuf,"Data truncation occurred!");
		return true;
	}
	else
		return false;
}


int   mysql_cursor::affectRec() const
{
	if(1 == BUFFER_ALL_RESULTS_TO_CLIENTSIDE)
      		return g_mysql.mysql_stmt_affected_rows(stmtp);
		//return mysql_affected_rows(&m_connect->mysql);
	else
		return m_row_count;
}

int mysql_cursor::prepare(const char *sql_str)
{
    if (g_mysql.mysql_stmt_prepare(stmtp, sql_str, strlen(sql_str)) != 0)
        return -1;
    return 0;
}

int mysql_cursor::execute(int iMode)
{
	if(iMode == MYSQL_SELECT_MODE)
	{
		 if( g_mysql.mysql_stmt_execute(stmtp) != 0)
		 	return -1;
		
		if(1 == BUFFER_ALL_RESULTS_TO_CLIENTSIDE)
			if (g_mysql.mysql_stmt_store_result(stmtp))
				return -1;
	}
	else
	{
		if( g_mysql.mysql_stmt_execute(stmtp) != 0)
	 		return -1;
	}
	
	
	return 0;
}


int mysql_cursor::getResultsRowCount()
{
	if(1 == BUFFER_ALL_RESULTS_TO_CLIENTSIDE && prepare_meta_result)
		return (int)g_mysql.mysql_num_rows(prepare_meta_result);
	else 
		return -1;
}


int mysql_cursor::fetch()
{
	m_row_count ++;
	return g_mysql.mysql_stmt_fetch(stmtp);
}

char* mysql_cursor::fetchBlobColumn(unsigned int col_pos)//Do not use this,mysql C API unsupport it now.
{
       int fixed_len = MAX_DATA_BUF_LENGTH;
       char buf[MAX_DATA_BUF_LENGTH];
	memset(buf, 0, sizeof(buf) );
	
	unsigned long fetched_len = 0;
	my_bool       is_null;
       MYSQL_BIND blob_bind;
	memset(&blob_bind,0,sizeof(MYSQL_BIND));
	blob_bind.buffer=buf;
	blob_bind.buffer_length=fixed_len;
	blob_bind.length=&fetched_len;
	blob_bind.buffer_type=MYSQL_TYPE_BLOB;
	blob_bind.is_null=&is_null;
	
	int ret;
	unsigned long offset=0;
	char* final_buf = NULL;
	while((ret=g_mysql.mysql_stmt_fetch_column(stmtp,&blob_bind,col_pos,offset)) == 0)
	{
		offset += fetched_len;
		char* tmp_buf = final_buf;
		final_buf = (char*)malloc(offset);
		if(tmp_buf)
		{
			mempcpy(mempcpy(final_buf,tmp_buf,offset-fetched_len),buf,fetched_len);
			free(tmp_buf);
		}
		else
		{
			memcpy(final_buf,buf,fetched_len);
		}
		
		if(fetched_len < MAX_DATA_BUF_LENGTH)
			break;
	}
	
	return final_buf;
}

int mysql_cursor::outputStru(TFieldList& fld_list)
{
	if(m_itemNum <= 0) 
		return(1);	
        fld_list.newFldList(m_itemNum);


	for(int n=0; n<m_itemNum; n++)
	{
		fld_list[n].setFieldName((const char *)mydefine[n].col_name);
		fld_list[n].setSourceType(mydefine[n].type);
	}

	return 0;
}



int mysql_cursor::output(TFieldList& fld_list)
{
	int i = 0;
	char* temp_buf = NULL;
	char buf[STRING_SIZE];
	memset(buf,0,sizeof(buf));

	if(m_itemNum <= 0) 
		return(1);	
	
	fld_list.newFldList(m_itemNum);



	for(i=0;i<this->m_itemNum;i++)
	{
		fld_list[i].setFieldName((const char *)(mydefine+i)->col_name);
		fld_list[i].setSourceType((unsigned int)(mydefine+i)->type);
		
		if ((mydefine+i)->is_null)    
		{
			fld_list[i].fieldValue().setNULL();
		}
		else
		{  	
			switch ((mydefine+i)->type)  	
			{
				case	MYSQL_TYPE_SHORT:  	    
				case	MYSQL_TYPE_LONG:
					//sprintf(buf,"%ld", *((int*)(mydefine+i)->buf));
					if(mybind[i].is_unsigned)
                                                    fld_list[i].fieldValue().setUInt(*((unsigned int*)(mydefine+i)->buf));
					else
                                                    fld_list[i].fieldValue().setInt(*((int*)(mydefine+i)->buf));
					break;
				case	MYSQL_TYPE_FLOAT:
					fld_list[i].fieldValue().setFloat(*((float*)(mydefine+i)->buf));
					break;
				case	MYSQL_TYPE_LONGLONG:
					//sprintf(buf,"%lld", *((long long*)(mydefine+i)->buf));
					if(mybind[i].is_unsigned)
						fld_list[i].fieldValue().setULong(*((unsigned long long*)(mydefine+i)->buf));
					else
						fld_list[i].fieldValue().setLLong(*((long long*)(mydefine+i)->buf));
					break;
				case      MYSQL_TYPE_VAR_STRING:        
				case      MYSQL_TYPE_STRING:
					fld_list[i].fieldValue().setString((char*)(mydefine+i)->buf);
					break;
				case     MYSQL_TYPE_DATETIME:
					/*
					sprintf(buf,"%04d%02d%02d%02d%02d%02d", 
							((MYSQL_TIME*)(mydefine+i)->buf)->year,
				                    ((MYSQL_TIME*)(mydefine+i)->buf)->month,
				                    ((MYSQL_TIME*)(mydefine+i)->buf)->day,
				                    ((MYSQL_TIME*)(mydefine+i)->buf)->hour,
				                    ((MYSQL_TIME*)(mydefine+i)->buf)->minute,
				                    ((MYSQL_TIME*)(mydefine+i)->buf)->second);
				       */
					date_format(buf,STRING_SIZE, m_dateformat,(MYSQL_TIME*)(mydefine+i)->buf);
					fld_list[i].fieldValue().setDate(buf);
					break;
				case	MYSQL_TYPE_TINY_BLOB:
				case	MYSQL_TYPE_MEDIUM_BLOB:
				case	MYSQL_TYPE_BLOB:
                                            fld_list[i].fieldValue().setBinary((char*)(mydefine+i)->buf, (mydefine+i)->length);
					/*
					temp_buf = this->fetchBlobColumn(i);
					if(temp_buf)
					{
						//ptr[i].value.value    = str_dup(temp_buf);
						//free(temp_buf);
						ptr[i].value.value = temp_buf;
					}
					*/
					break;
				default:
                                            fld_list[i].fieldValue().setValue((char*)(mydefine+i)->buf, (mydefine+i)->length);
			}
		}
	}
	
	return 0;
}


mysqlapi::mysqlapi()
{
	m_username[0]=0;
	m_passwd[0]=0;
	m_hostname[0]=0;
	now_operation = system;
	d_stmt_err_no= 0;
    	d_stat_msgbuf[0]='\0';
	this->phfree();
}


mysqlapi::~mysqlapi()
{
	this->m_curlist.free();
}

int	mysqlapi::getErr()
{	
	if(now_operation == system)
		return this->m_connect.getErr();
	else if(now_operation == statement)
		return (this->m_cursor.getStmtErrNo() == 0)?this->m_connect.getErr():this->m_cursor.getStmtErrNo();
	else if(now_operation == dynamic_statement)
		return (d_stmt_err_no == 0)?this->m_connect.getErr():d_stmt_err_no;
}

char*	mysqlapi::getErrInfo()
{
	if(now_operation == system)
		return this->m_connect.getErrInfo();
	else if(now_operation == statement)
		return (this->m_cursor.getStmtErrNo() == 0)?this->m_connect.getErrInfo():this->m_cursor.getStmtErrInfo();
	else if(now_operation == dynamic_statement)
		return (d_stmt_err_no == 0)?this->m_connect.getErrInfo():d_stat_msgbuf ;
}

void mysqlapi::phfree()
{
	now_operation = system;
	m_sql[0] = 0;
	memset(m_bin_fld_val_list,0,sizeof(mysql_fld_val_t)*32);
	for(int i=0; i<32; i++)
	{
	 	m_bin_fld_val_list[i].data = NULL;
	}
	return;
}

int 	mysqlapi::connect( char *user,char *password,char *hoststring )
{
	now_operation = system;
	if(strlen(user) > sizeof(m_username)-1 || 
	   strlen(password) > sizeof(m_passwd)-1 || 
	   strlen(hoststring) > sizeof(m_hostname)-1 )
	{
		m_username[0] = '\0';
        	return -1;
	}
	else
	{
	       strcpy(m_username, user);
	       strcpy(m_passwd, password);
	       strcpy(m_hostname, hoststring);
	}
	
	return m_connect.connect((const char*)user,(const char*)password,(const char*)hoststring);
}


int  	mysqlapi::disconnect()
{
	now_operation = system;
	m_username[0]=0;
	m_cursor.closeCur();
	return	m_connect.disconnect();
}


void mysqlapi::beginTrans()
{
	//now_operation = system;
	m_connect.beginTrans();
}


void mysqlapi::commitTrans()
{
	//now_operation = system;
	m_connect.commitTrans();
}


void mysqlapi::rollbackTrans()
{	
	//now_operation = system;
	m_connect.rollbackTrans();
}


int   mysqlapi::changeDB(const char* db_name)
{
	now_operation = system;
	return m_connect.changeDB(db_name);
}


int   mysqlapi::processDdl(const char* ddl_sql)
{
	now_operation = system;
	return m_connect.processDdl(ddl_sql);
}

int mysqlapi::inSetSql(const char *sql)
{
	now_operation = statement;
	
	m_cursor.closeCur();
	if(m_cursor.openCur(&m_connect)<0)
		return -1;

	if(m_cursor.prepare(sql)<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	return 0;
}


int mysqlapi::inExec(bool isCommit)
{
	now_operation = statement;
	
	if(m_cursor.execute(MYSQL_NON_SELECT_MODE)<0)
	{
		m_cursor.getStmtErrInfo();
		this->rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}
	
	if(isCommit == true)
	{
		this->commitTrans();
	}
	
	m_cursor.closeCur();
	return 0;
}


int mysqlapi::setLongFld(const char *sql,char *longMsg,unsigned long length,int longType,bool isCommit)
{
	now_operation = statement;
	
	if(!longMsg)
		return -1;
	
	if(this->phSetSql(sql)<0)
		return -1;

	if(this->inSetSql(m_sql)<0)
		return -1;


	if(m_cursor.allocateParmBindStruct()<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	unsigned long parm_len=0,buf_len;

	switch(longType)
	{
		case	SQL_BINARY:
			buf_len = length;
			m_cursor.bindPos(0,longMsg, MYSQL_TYPE_BLOB, length,&parm_len);
			break;
		case	SQL_TEXT:
			buf_len = strlen(longMsg);
			m_cursor.bindPos(0,longMsg, MYSQL_TYPE_STRING, strlen(longMsg),&parm_len);
			break;
		default:
			m_cursor.closeCur();
			return -1;
	}

	if(m_cursor.bindParm() != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}	
	
	if(m_cursor.bindLargeParm(0, longMsg, buf_len) != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}
	

	if(this->inExec(isCommit)<0)
		return -1;

	return 0;
}


int mysqlapi::beginLoadBinaryData(const char* sql,int col_type)
{
	now_operation = statement;
	
	if(this->phSetSql((char*)sql)<0)
		return -1;

	if(this->inSetSql(m_sql)<0)
		return -1;


	if(m_cursor.allocateParmBindStruct()<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	m_bin_data_parm_len=0;

	switch(col_type)
	{
		case	SQL_BINARY:
			m_cursor.bindPos(0,0, m_cursor.get_datatype_BLOB_id(), 0,&m_bin_data_parm_len);
			break;
		case	SQL_TEXT:
			m_cursor.bindPos(0,0, m_cursor.get_datatype_TEXT_id(), 0,&m_bin_data_parm_len);
			break;
		default:
			m_cursor.closeCur();
			return -1;
	}

	if(m_cursor.bindParm() != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}
	
	return 0;
}


int mysqlapi::transferBinData(const char * pdata,unsigned long data_len)
{
	now_operation = statement;
	
	if(m_cursor.bindLargeParm(0, pdata, data_len) != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	return 0;
}


int mysqlapi::endLoadBinaryData(bool is_commit)
{
	now_operation = statement;
	
	if(this->inExec(is_commit)<0)
		return -1;

	return 0;
}


int mysqlapi::setDateFormat(DateFormat dateFormat)
{
	now_operation = system;
	/*
	int rtn=-1;
	
	switch (dateFormat){
	case default_date :
		rtn = processDdl((const char *)"set @@session.datetime_format='%Y-%m-%d %H:%i:%s'");
		break;
	case chinese_date:
		rtn = processDdl((const char *)"set @@session.date_format='%Y-%m-%d'");
		break;
	case chinese_datetime:
		rtn = processDdl((const char *)"set @@session.datetime_format='%Y-%m-%d %H:%i:%s' ");
		break;
	default:
		rtn = processDdl((const char *)"set @@session.date_format='%d-%m-%Y' ");
		break;
	}
	*/
	m_cursor.setDateFormat(dateFormat);
	return 0;
}


bool mysqlapi::isTable(const char *tab_name)
{
	now_operation = statement;
	
	char	sql[STRING_SIZE];	
	snprintf(sql,STRING_SIZE,"show tables like '%s' ",tab_name);
	//snprintf(sql,STRING_SIZE,"SELECT table_name FROM INFORMATION_SCHEMA.TABLES where table_name='%s' ",tab_name);

	if(m_cursor.openCur(&m_connect)<0)
		return false;
	
	if(m_cursor.prepare(sql)<0)
	{
		m_cursor.closeCur();
		return false;
	}

	if(m_cursor.define()<0)
	{
		m_cursor.closeCur();
		return false;
	}

	if(m_cursor.execute(MYSQL_SELECT_MODE)<0)
	{
		m_cursor.closeCur();
		return false;
	}

	if(m_cursor.fetch() != 0)
	{
		m_cursor.closeCur();
		return false;
	}

	m_cursor.closeCur();

	return true;
}


bool mysqlapi::checkSql(const char *pcSQLStmt, bool isSelectSQL)
{
	now_operation = statement;
	
	if(this->inSetSql(pcSQLStmt)<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return false;
	}
	
	m_cursor.closeCur();
	
	return true;
}


int mysqlapi::getRec(const char *sql,TFieldList& fld_list)
{
	now_operation = statement;
	
	int ret;
	
	if(m_cursor.openCur(&m_connect)<0)
		return -1;
	
	if(m_cursor.prepare(sql)<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	if(m_cursor.define()<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	if(m_cursor.execute(MYSQL_SELECT_MODE)<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	if((ret=m_cursor.fetch()) != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		if(m_cursor.isStmtResultNoData(ret))
			return -3;
		else
			return -1;
	}

	m_cursor.output(fld_list);

	m_cursor.closeCur();

	return 0;
}


int mysqlapi::getSysDate(char *pcSysdate)
{
	now_operation = statement;
	
	if(!pcSysdate)
		return -1;
	
	TFieldList my_list;

	if(getRec((char *) "select now()", my_list)<0)
		return -1;

	if(!my_list[0].fieldValue().isNULL())
	{
		return -1;
	}
	
	strcpy(pcSysdate, my_list[0].fieldValue().asDate());


	return 0;
}


int mysqlapi::executeSql(const char *sql,int *iReturn, bool isCommit)
{
	now_operation = statement;
	
	if(m_cursor.openCur(&m_connect)<0)
		return -1;
	
	if(m_cursor.prepare(sql)<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	if(m_cursor.execute(MYSQL_NON_SELECT_MODE)<0)
	{
		this->rollbackTrans();
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		return -1;
	}

	if(iReturn)
		*iReturn = m_cursor.affectRec();

	if(isCommit == true)
		this->commitTrans();

	m_cursor.closeCur();

	return 0;
}


int mysqlapi::checkConStatus(int reconInterval, int maxRetryTimes)
{
	now_operation = system;
	unsigned int err_no = m_connect.getErr();

	if(m_connect.isConnect() == 0 && -99999 != err_no)
		return -1;

	for(int try_time = 0;try_time<maxRetryTimes;try_time++)
	{
		m_connect.setNoConnct();

		if(m_connect.connect(this->m_username, this->m_passwd, this->m_hostname) == 0)
			return try_time;

		m_curlist.free();
		sleep(reconInterval);
	}

	return -99999;
}


int mysqlapi::query(const char *sql,int& handle)
{
	now_operation = dynamic_statement;
	
	if(!sql)
		return -1;
	
	mysql_cursor * ptr = NULL;
	int hd = m_curlist.add(1);
	
	if((ptr = m_curlist.find(hd)) == NULL)
		return -1;
	
	if(ptr->openCur(&m_connect)<0)
	{
		ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);
		m_curlist.del(hd);
		
		return -1;
	}
	
	if(ptr->prepare(sql)<0)
	{
		ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);
		ptr->closeCur();		
		m_curlist.del(hd);
		return -1;
	}

	if(ptr->define()<0)
	{
		ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);
		ptr->closeCur();
		m_curlist.del(hd);
		return -1;
	}

	if(ptr->execute()<0)
	{
		ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);
		ptr->closeCur();
		m_curlist.del(hd);
		return -1;
	}

	handle = hd;

	return 0;
}


int mysqlapi::fetch(int handle,TFieldList& fld_list)
{
	now_operation = dynamic_statement;
	int result;
	mysql_cursor * ptr = m_curlist.find(handle);
	
	if(!ptr)
		return -1;

	if((result=ptr->fetch()) != 0)
	{
		
		if(ptr->isStmtResultNoData(result))
		{
			ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);	
			//printf("-3Test fetch error info:%s\n",ptr->getStmtErrInfo());
			return -3;
		}
		else
		{
			ptr->reportErrorInfo(&d_stmt_err_no, d_stat_msgbuf);
			//printf("-1Test fetch error info:%s\n",ptr->getStmtErrInfo());
			return -1;
		}
	}
	
	ptr->output(fld_list);
	
	return 0;
}


int mysqlapi::noValueFetch(int handle)
{
    TFieldList fld_list;
    return fetch(handle, fld_list);
}


int mysqlapi::getCurRec(int handle,int* rowCount)
{
	now_operation = dynamic_statement;
	mysql_cursor * ptr = m_curlist.find(handle);
	
	if(!ptr)
		return -1;
	
	*rowCount = ptr->affectRec();

	return 0;
}

int mysqlapi::closequery(int handle)
{
	now_operation = dynamic_statement;
	mysql_cursor * ptr = m_curlist.find(handle);
	
	if(!ptr)
		return -1;

	ptr->closeCur();
	m_curlist.del(handle);

	return 0;
}


int mysqlapi::phSetSql(const char *sql)
{
	now_operation = statement;
	
	if(!sql || strlen(sql)>=MAX_DATA_BUF_LENGTH)
		return -1;

	this->phfree();

	strncpy(m_sql,sql,MAX_DATA_BUF_LENGTH);

	return 0;
}


int  mysqlapi::phsetParm(unsigned int n,DataType d_type,void* value,int len)
{
	now_operation = statement;
	
	if(!value)
		return -1;
	if(n<0 || n>32)
		return -1;

	m_bin_fld_val_list[n].type = d_type;
	m_bin_fld_val_list[n].data = value;
	if(len != 0)
		m_bin_fld_val_list[n].len = len;

	return 0;
}


int mysqlapi::phExecute(bool isCommit)
{
	if(m_connect.isConnect()<0)
		return -1;
	now_operation = statement;
	
	if(inSetSql(m_sql)<0)
		return -1;

	if(m_cursor.allocateParmBindStruct()<0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		phfree();
		return -1;	
	}

	for(int i=0;i<32;i++)
	{
		if(!m_bin_fld_val_list[i].data)
			break;

		switch (m_bin_fld_val_list[i].type)
		{
			case	SQL_INT:
				m_cursor.bindPos(i,(char*)m_bin_fld_val_list[i].data,MYSQL_TYPE_LONG,sizeof(int));
				break;
			case	SQL_LONG:
				m_cursor.bindPos(i,(char*)m_bin_fld_val_list[i].data,MYSQL_TYPE_LONGLONG,sizeof(long long));
				break;
			case	SQL_STRING:
				m_bin_fld_val_list[i].data_len = strlen((const char*)m_bin_fld_val_list[i].data);
				m_cursor.bindPos(i,(char*)m_bin_fld_val_list[i].data,MYSQL_TYPE_STRING,m_bin_fld_val_list[i].len,&m_bin_fld_val_list[i].data_len);
				break;
			case	SQL_BINARY:
				m_cursor.bindPos(i,(char*)m_bin_fld_val_list[i].data,MYSQL_TYPE_BLOB,m_bin_fld_val_list[i].len);
				break;
			default:
				m_cursor.closeCur();
				phfree();
				return -1;
		}
	}
	
	if(m_cursor.bindParm() != 0)
	{
		m_cursor.getStmtErrInfo();
		m_cursor.closeCur();
		phfree();
		return -1;
	}

	if(inExec(isCommit)<0)
		return -1;

	phfree();

	return 0;
}


