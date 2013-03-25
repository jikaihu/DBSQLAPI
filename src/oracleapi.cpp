#include "oracleapi.h"
#include "orahandle.h"

#define FAILOVER
//#define FAILOVER_MODE

oraHandle g_oracle;

 
sb4  callback_fn(dvoid * svchp, dvoid * envhp,dvoid * fo_ctx,ub4  fo_type,ub4  fo_event)
{
        OCIError *errhp;
        g_oracle.OCIHandleAlloc(envhp, (dvoid **)&errhp, (ub4) OCI_HTYPE_ERROR,
              (size_t) 0, (dvoid **) 0);
 
 
	switch (fo_event)
	{
	   case OCI_FO_BEGIN:
	   {
	     printf(" Failover type was found to be %s \n",
	                     ((fo_type==OCI_FO_SESSION) ? "SESSION"
	                     :(fo_type==OCI_FO_SELECT) ? "SELECT"
	                     : "UNKNOWN!"));
	     printf(" Failover Context is :%s\n",
	                    (fo_ctx?(char *)fo_ctx:"NULL POINTER!"));
	     break;
	   }
	   case OCI_FO_ABORT:
	   {
	     printf(" Failover stopped. Failover will not take place.\n");
	     break;
	   }
	   case    OCI_FO_END:
	   {
	       printf(" Failover ended ...resuming services\n");
	     g_oracle.OCITransRollback((OCISvcCtx *)svchp, errhp, OCI_DEFAULT);
	     break;
	   }
	   case OCI_FO_REAUTH:
	   {
	       printf(" Failed over user. Resuming services\n");
	     break;
	   }
	   default:
	   {
	     printf("Bad Failover Event: %d.\n",  fo_event);
	     break;
	   }
	}
	
        g_oracle.OCIHandleFree((dvoid *)errhp, (ub4)OCI_HTYPE_ERROR);
		
   	return 0;
}
 
int register_callback(dvoid * srvh, OCIError *errh)
{
	  OCIFocbkStruct failover;                
	  
	  if (!(failover.fo_ctx = (dvoid *)malloc(strlen("dbapi context.")+1)))
	  	return(1);
	  	  
	  strcpy((char *)failover.fo_ctx, "dbapi context.");
	  failover.callback_function = &callback_fn;
	  	  
	  if (g_oracle.OCIAttrSet(srvh, (ub4) OCI_HTYPE_SERVER,
	                (dvoid *) &failover, (ub4) 0,
	                (ub4) OCI_ATTR_FOCBK, errh)  != OCI_SUCCESS)
	  	return(2);
	  
	  return (0);
}


db_connect::db_connect() 
{ 
    m_state		=	not_connected; 
    init_mode   =   OCI_THREADED;        //OCI_DEFAULT
    credt		=	(ub4)OCI_CRED_RDBMS;
    g_oracle.loadLibrary();
}

db_connect::~db_connect() 
{
    if(m_state == connected)
    {
        disconnect();
    }
}

//释放指定连接句柄
void db_connect::freeHandle()
{
    if(srvhp)
    {
        (void)g_oracle.OCIHandleFree((dvoid *)srvhp, (ub4)OCI_HTYPE_SERVER);
        srvhp = NULL;
    }

    if(svchp)
    {
        (void)g_oracle.OCIHandleFree((dvoid *)svchp, (ub4)OCI_HTYPE_SVCCTX);
         svchp = NULL;
    }

    if(errhp) 
    {
        (void)g_oracle.OCIHandleFree((dvoid *)errhp, (ub4)OCI_HTYPE_ERROR);
        errhp = NULL;
    }

    if(authp)
    {
        (void)g_oracle.OCIHandleFree((dvoid *)authp, (ub4)OCI_HTYPE_SESSION);
        authp = NULL;
    }

    if(envhp)
    {
        (void)g_oracle.OCIHandleFree((dvoid *)envhp, (ub4)OCI_HTYPE_ENV);
        envhp = NULL;
    }
  return;
}

//初始化指定句柄
int db_connect::initHandle()
{
  // 初始化 OCI 进程 
	if(g_oracle.OCIInitialize(init_mode,(dvoid *)0,(dvoid * (*)(dvoid *, size_t)) 0,
                    (dvoid * (*)(dvoid *, dvoid *, size_t))0,(void (*)(dvoid *, dvoid *)) 0))
	{
		return -1;
	}

  // 初始化 OCI 环境 
	if(g_oracle.OCIEnvInit((OCIEnv **)&envhp,(ub4)OCI_DEFAULT,(size_t)0,(dvoid **)0))
	{
		return -1;
	}

  // 申请 service 句柄 
	if(g_oracle.OCIHandleAlloc((dvoid *) envhp, (dvoid **) &svchp, (ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
	{
		return -1;
	}
 
  // 申请  error 句柄 
	if(g_oracle.OCIHandleAlloc((dvoid *) envhp, (dvoid **) &errhp, (ub4) OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0))
	{
		return -1;
	}
 
  // 申请 service 句柄 
	if(g_oracle.OCIHandleAlloc((dvoid *) envhp, (dvoid **) &srvhp, (ub4) OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0))
	{
		return -1;
	}

  // 申请 authentication 句柄 
	if(g_oracle.OCIHandleAlloc((dvoid *) envhp, (dvoid **) &authp, (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0))
	{
		return -1;
	}

	return 0;
}

//用给定的模式绑定服务器
int db_connect::attachServer(ub4 mode,OCIServer *srvhp,OCIError *errhp,OCISvcCtx *svchp,char *hoststring)
{
	if(!hoststring)
	{
		if(g_oracle.OCIServerAttach(srvhp, errhp, (text *)"", (sb4) strlen((char *)""), (ub4) OCI_DEFAULT))
		{
			return -1;
		}
	}
	else
	{
		if(g_oracle.OCIServerAttach(srvhp, errhp, (text *) hoststring, (sb4) strlen((char *)hoststring), (ub4) OCI_DEFAULT))
		{
			return -1;
		}
	}

  // 在服务句柄中设置服务器句柄 
	if(g_oracle.OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,(dvoid *) srvhp, (ub4) 0, (ub4) OCI_ATTR_SERVER,errhp))
	{
		return -1;
	}

	return 0;
}


//用给定的模式解开与服务器的绑定
void db_connect::deatchServer()
{
	if(g_oracle.OCISessionEnd(svchp, errhp, authp, (ub4) 0))
	{
	}

	if(g_oracle.OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT))
	{
	}

	return;
}

// 重设连接状态到 not_connected.
void db_connect::setNoConnct(void)
{
	m_state = not_connected;
}

int db_connect::connect(char *username, char *password, char *hoststring)
{

	int errcode;
	if(m_state == connected)
	{
		return -1;
	}
  //初始化环境变量
	if(initHandle())
	{
						 g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
             freeHandle();
             return -1;

	}

  // 绑定到数据库服务器
	if(attachServer((ub4) OCI_DEFAULT, srvhp, errhp, svchp, hoststring))
	{
            g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
            freeHandle();
            return -1;
	}

  // 在authentication句柄中设置属性 
  if(g_oracle.OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) username, (ub4) strlen((char *) username),
                 (ub4) OCI_ATTR_USERNAME, errhp))
	{
            g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
			g_oracle.OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT);
			freeHandle();
		        return -1;
	 }

	if(g_oracle.OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) password, (ub4) strlen((char *) password),
                 (ub4) OCI_ATTR_PASSWORD, errhp))
	{
            g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
		g_oracle.OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT);
		freeHandle();
	         return -1;
	}

	if(g_oracle.OCISessionBegin(svchp, errhp, authp, credt, OCI_DEFAULT))
	{
            g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
	   g_oracle.OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT);
	   freeHandle();
            return -1;
	}

  // 在Service句柄中设置authentication句柄 
  if(g_oracle.OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) authp, (ub4) 0, (ub4) OCI_ATTR_SESSION, errhp))
	{
            g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
	    deatchServer();
	    freeHandle();
	    return -1;
	}
    
	m_state = connected;
	m_transaction = NO;

	#ifdef FAILOVER_MODE
                register_callback(srvhp,errhp);
        #endif
	
	return  0;
}

int db_connect:: isConnect(void)
{
    if(m_state == not_connected)
    {
        return -1;
    }
    return 0;
}

int db_connect::disconnect()
{
	if(m_state == not_connected)
	{
		return -1;
	}
	
	deatchServer();	//断连服务器

	freeHandle();	//释放指定连接句柄

	m_state = not_connected;

  return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void db_connect::beginTrans()
{
	int status;
	if(m_transaction) status = g_oracle.OCITransCommit(svchp, errhp, OCI_DEFAULT);
	m_transaction = YES;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void db_connect::commitTrans()
{
	int status = g_oracle.OCITransCommit (svchp, errhp, OCI_DEFAULT);
	m_transaction = NO;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void db_connect::rollbackTrans()
{
	int status = g_oracle.OCITransRollback(svchp, errhp, OCI_DEFAULT);
	m_transaction = NO;
}

int db_connect::getErr()
{
    sb4   errcode = 0;
    (void) g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
    
    if(errcode == 3113)	//网络断掉或数据库down
    {
  	    return -99999;
    } 
    else if(errcode == 3114)	//网络断掉或数据库down
    {
        return -99999;
    }
    else if(errcode == 28547)	//connection to server failed, probable Oracle Net admin error
    {
        return -99999;
    }
    else if(errcode == 12541)	//no listener
    {
        return -99999;
    }
    else if(errcode == 1012)	//not logged on
    {
        return -99999;
    }	 
    else if(errcode == 25402)	//transaction must roll back
    {
    	rollbackTrans();
         return errcode;
    }		
    else
    {
        return errcode;
    }
    
}
char* db_connect::getErrInfo()
{
    if( not_connected == m_state)
    {
        return (char*)msgbuf;
    }
    else
    {
        sb4   errcode = 0;
        msgbuf[0] = 0;
        (void) g_oracle.OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,msgbuf,(ub4)sizeof(msgbuf),(ub4)OCI_HTYPE_ERROR);
        return (char*)msgbuf;
    }
}


db_cursor::db_cursor() 
{   
	m_state = not_opened; 
	m_connect = (db_connect *)0; 

	initDef(); 
}
 
db_cursor::~db_cursor()
{
	if(m_state == opened) closeCur();

	freeDef(); 
}

int db_cursor::openCur(db_connect *conn_param)
{
	if(m_state == opened)
	{
		return -1;
	}
  
	if(g_oracle.OCIHandleAlloc((dvoid *)conn_param->envhp,(dvoid **) &m_ociStmtp,(ub4)OCI_HTYPE_STMT,(CONST size_t)0,(dvoid **)0))
	{
		m_state = not_opened;
		m_connect = conn_param;
		return -1;
	}
	else
	{
		m_state = opened;
		m_connect = conn_param;  // set m_state.
	}
	return 0;
}

// 关闭 Cursor
int db_cursor::closeCur()
{
	int status=0;

	if(m_state == not_opened)
	{
		return -1;
	}

    if(m_ociStmtp)
    {
        (void)g_oracle.OCIHandleFree( (dvoid *) m_ociStmtp, (ub4) OCI_HTYPE_STMT);
        m_ociStmtp = NULL;
    }

	m_state = not_opened;
	m_connect = (db_connect *)0;
  
	return (status);
}


// 绑定输入变量（字段号）
int db_cursor::bindPos(int position,ub1	*progvar,int progvarlen,int datatype,int scale,sb2 *indicator)
{  
	int status = g_oracle.OCIBindByPos(m_ociStmtp,//statement handle to the SQL or PL/SQL statement being processed
								(OCIBind **)&bndhp, // an address of a bind handle which is implicitly allocated by this call
								m_connect->errhp,//error handle
								position,//the placeholder attributes are specified by position 
								(dvoid *) progvar,//an address of a data value or an array of data values 
								(sb4) progvarlen,//the size of a data value
								datatype, //the data type of the value(s) being bound
								0,//indicator,//pointer to an indicator variable or array
								0,//pointer to array of actual lengths of array elements
								0,//pointer to array of column level return codes
								0,//the maximum possible number of elements of type 
								0,//a pointer to the actual number of elements 
								OCI_DEFAULT);//the valid modes 

	return status;
}
    

// 定义输出变量（字段列）
int db_cursor::define(OCIDefine *defnp,int position,ub1	*buf,int bufl, 
					 int datatype,sb2 *indicator,ub2 *rlen,ub2 *rcode,ub4 mode)
{ 
	return  g_oracle.OCIDefineByPos(m_ociStmtp,(OCIDefine **) &defnp,m_connect->errhp,
		  (ub4) position,(dvoid *) buf,(sb4) bufl,(ub2) datatype,
		  (dvoid *) indicator,(ub2 *) rlen,(ub2 *) rcode,(ub4) mode);
}
    
int db_cursor::implicitDefine()
{
	int  defLen, defType;
	ub1 *defPtr;
	ub4 define_mode=OCI_DEFAULT;//declare define mode default is OCI_DEFAULT
	memset(m_describe, '\0', sizeof(m_describe));
	freeDef(); 
	OCIParam	*parmdp;
	text		*namep;
	ub4			sizep;
	sb4			param_status;

	ub4 pos = 1;
   if((param_status = g_oracle.OCIParamGet(m_ociStmtp, (ub4)OCI_HTYPE_STMT, m_connect->errhp,(dvoid **) &parmdp,(ub4) pos)))
					   return -1;

	//loop only if a descriptor was successfully retrieved for current position, starting at 1
	for(;param_status==0;)
	{
		// 数据类型 
		g_oracle.OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
			 (dvoid*) &m_describe[pos-1].dbType, (ub4 *) 0,(ub4) OCI_ATTR_DATA_TYPE,(OCIError *)m_connect->errhp);
                                             
		// 列长度 
    	if(g_oracle.OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM, 
			  (dvoid*) &m_describe[pos-1].dbSize, (ub4 *) 0, 
			  (ub4) OCI_ATTR_DATA_SIZE, (OCIError *)m_connect->errhp) )
			  break;

		// 列名 
		if(g_oracle.OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) &namep, (ub4 *) &sizep, 
					(ub4) OCI_ATTR_NAME, (OCIError *)m_connect->errhp) )
					break;

		strncpy((char *)m_describe[pos-1].buf, (char *)namep, (size_t) sizep);
    	m_describe[pos-1].buf[sizep] = '\0';
		m_describe[pos-1].bufLen = sizep;

		// precision 
    	g_oracle.OCIAttrGet((dvoid*)parmdp,(ub4)OCI_DTYPE_PARAM,(dvoid*)&m_describe[pos-1].precision,(ub4 *)0,
			  (ub4)OCI_ATTR_PRECISION,(OCIError *)m_connect->errhp);
                                                      
		// scale 
		g_oracle.OCIAttrGet((dvoid*)parmdp,(ub4)OCI_DTYPE_PARAM,(dvoid*)&m_describe[pos-1].scale,(ub4 *)0,
						(ub4)OCI_ATTR_SCALE,(OCIError *)m_connect->errhp);


		switch(m_describe[pos-1].dbType)
		{
			case OCI_TYPECODE_NUMBER:
				m_describe[pos-1].dbSize = 30;
				if(m_describe[pos-1].scale != 0 ) {
					defPtr  = (ub1 *) &m_define[pos-1].fltBuf;
					defLen  = (int) sizeof(double);
					defType = SQLT_FLT;
					m_describe[pos-1].dbType = defType;
				}else if(m_describe[pos-1].scale == 0 && m_describe[pos-1].precision != 0){
    				if(m_describe[pos-1].precision < 10)
    				{
    					defPtr  = (ub1 *) &m_define[pos-1].intBuf;
    					defLen  = (int) sizeof(int);
    					defType = SQLT_INT;
    					m_describe[pos-1].dbType = defType;
                    }
                    else
                    {
                        defPtr = (ub1 *) &m_define[pos-1].numBuf;
    					defLen  = (int) sizeof(OCINumber);
    					defType = SQLT_VNU;
    					m_describe[pos-1].dbType = defType;
                    }       
				}else{//scale=0 && precision=0
					defPtr  = (ub1 *) &m_define[pos-1].fltBuf;
					defLen  = (int) sizeof(double);
					defType = SQLT_FLT;
					m_describe[pos-1].dbType = defType;
				}
				define_mode=OCI_DEFAULT;
				break;
			case DATA_TYPE_LONG: //long column , m_describe[n].dbType = 8
				defLen = 65536;
				defPtr = m_define[pos-1].buf;
				defType = SQLT_LNG;
				define_mode=OCI_DYNAMIC_FETCH;
				break;
			case DATA_TYPE_LONGBINARY://long raw column ,m_describe[n].dbType = 24
				defLen = 65536;
				defPtr = m_define[pos-1].buf;
				defType = SQLT_LBI;
				define_mode=OCI_DYNAMIC_FETCH;
				break;
			case DATA_TYPE_BINARY:
				defLen = m_describe[pos-1].dbSize + 1 ;
				m_define[pos-1].buf = new unsigned char[defLen];

				if(m_define[pos-1].buf) 
				{
                    defPtr = m_define[pos-1].buf;
				}
				defType = SQLT_BIN;
				define_mode=OCI_DEFAULT;
				break;
			default:
				if(m_describe[pos-1].dbType == OCI_TYPECODE_DATE) m_describe[pos-1].dbSize = 20;

				defLen = m_describe[pos-1].dbSize + 1;
				m_define[pos-1].buf = new unsigned char[defLen];
				if(m_define[pos-1].buf) defPtr = m_define[pos-1].buf;

				defType = SQLT_STR;
				define_mode=OCI_DEFAULT;
				break;
		}

		if(define((OCIDefine *) NULL,pos,defPtr,defLen,defType,&m_define[pos-1].indp,&m_define[pos-1].colRetLen,
			&m_define[pos-1].colRetCode,define_mode) )
		{
			return -1;
		}

		pos++;
	   param_status = g_oracle.OCIParamGet(m_ociStmtp,(ub4)OCI_HTYPE_STMT,m_connect->errhp,(dvoid **)&parmdp,(ub4)pos);
	}

	m_itemNum = pos - 1;

	return 0;
}


int db_cursor::define()
{
	int status;

	//第一次 execute, 然后 describe
	status = implicitDefine();	

	return(status); 
}


//------------------------------------------------------------------------------
// 初始化结构定义
//------------------------------------------------------------------------------
void db_cursor::initDef()
{
	for(int n=0; n<MAXITEMS; n++)
	{
		m_define[n].intBuf = 0;
		m_define[n].fltBuf = 0.0;
		m_define[n].buf = NULL;
		m_define[n].indp = 0;
		m_define[n].colRetCode = 0;
		m_define[n].colRetLen = 0;
	}

	return;
}

//--------------------------------------------------------------
//释放申请的def结构内存
//--------------------------------------------------------------
void db_cursor::freeDef()
{
	for(int n=0; n<MAXITEMS; n++)
	{
		m_define[n].intBuf = 0;
		m_define[n].fltBuf = 0.0;
		m_define[n].indp = 0;
		m_define[n].colRetCode = 0;
		m_define[n].colRetLen = 0;

		if(m_define[n].buf)
		{
			delete [] m_define[n].buf; 
			m_define[n].buf = NULL;
		}
	}
}


int db_cursor::prepare(const char *pcSQLStmt)
{
  if(g_oracle.OCIStmtPrepare(m_ociStmtp,m_connect->errhp,(unsigned char *)pcSQLStmt,(ub4)strlen((char *)pcSQLStmt),
		(ub4)OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT))
	{
		return -1;
	}

	return 0;
}


int db_cursor::execute(int iMode)
{ 
	int status;

	if(iMode == SELECT_MODE){//select mode
		status = g_oracle.OCIStmtExecute (m_connect->svchp,m_ociStmtp,m_connect->errhp,0,	//iters, for select, prefetch row to buffer
								 0,//rowoff,
								(CONST OCISnapshot*) NULL,(OCISnapshot*)NULL,OCI_DEFAULT);
	}
	else
	{//insert, delete, update mode
		status = g_oracle.OCIStmtExecute (m_connect->svchp,m_ociStmtp,m_connect->errhp,1,0,//rowoff,
								(CONST OCISnapshot*)NULL,(OCISnapshot*)NULL,OCI_DEFAULT);
	}

	return (status); 
}


int db_cursor::fetch()
{ 
	return g_oracle.OCIStmtFetch(m_ociStmtp,m_connect->errhp,1,OCI_DEFAULT,OCI_DEFAULT);
}


char *db_cursor::fetchDefineFld()
{
	dvoid	*hndlpp;//句柄
	ub1		in_outp;//返回 OCI_PARAM_IN 或 OCI_PARAM_OUT
	ub4		iterp;//r返回的行数
	ub4		typep;//hndlpp的句柄类型
	ub1		piecep;//返回 - OCI_ONE_PIECE, OCI_FIRST_PIECE...等定义的值
	ub2		rcodep;
	ub4		idxp;//PL/SQL 数组索引

	ub4		alenp=1024;//每次fetched的字符数
	ub4		buf[1025];//每次存放的字符数

	memset(buf, 0, sizeof(buf) );
	char	*pcRtnBuf=NULL; 
	int		iLength=1;//存放 ('\0')

	int status = OCI_NEED_DATA;
	while(status == OCI_NEED_DATA){
		status = g_oracle.OCIStmtGetPieceInfo(m_ociStmtp,m_connect->errhp,&hndlpp,&typep,&in_outp,&iterp,&idxp,&piecep);
		status = g_oracle.OCIStmtSetPieceInfo(hndlpp,typep,m_connect->errhp,buf,&alenp,piecep,&idxp,&rcodep);
		status = g_oracle.OCIStmtFetch(m_ociStmtp,m_connect->errhp,1,OCI_DEFAULT,OCI_DEFAULT);
		
		iLength = iLength + alenp;//当前要申请的内存的大小
		char *pcTmpBuf = pcRtnBuf;
		if( (pcRtnBuf = new char[iLength]) == NULL)	return NULL;
		if(pcTmpBuf)
		{
			strcpy(pcRtnBuf, pcTmpBuf);
			if(buf) strncat(pcRtnBuf, (char *)buf, alenp);

			delete [] pcTmpBuf;
		}
		else
		{
			strcpy(pcRtnBuf, (char *)buf);
		}
		
		if(alenp < 1024) break; 
		if(status == 0) break;
	}

	return(pcRtnBuf);
}


int db_cursor::outputStru(TFieldList& fld_list)
{
	if((m_itemNum <= 0)||(m_itemNum >MAXITEMS)) return(1);

	fld_list.newFldList(m_itemNum);


	for(int n=0; n<m_itemNum; n++)
	{
		fld_list[n].setFieldName((const char *)m_describe[n].buf);
		fld_list[n].setSourceType(m_describe[n].dbType);
	}

	return 0;
}


int db_cursor::output(TFieldList& fld_list)
{
	int n, rtn;
        long long tmpnum;// michael 
	TField *ptr;
	char buf[41], *pcLongColumn;
	int iLength;
	memset(buf,0,sizeof(buf));

	if((m_itemNum <= 0)||(m_itemNum >MAXITEMS)) return(1);

	fld_list.newFldList(m_itemNum);

	for(n=0; n<m_itemNum; n++)
	{
		fld_list[n].setFieldName((const char *)m_describe[n].buf);
		fld_list[n].setSourceType(m_describe[n].dbType);
		if(m_define[n].indp <0)
		{
			fld_list[n].fieldValue().setNULL();
		}
		else
		{
			switch(m_describe[n].dbType)
			{
				case OCI_TYPECODE_FLOAT:
					fld_list[n].fieldValue().setFloat(m_define[n].fltBuf);
					break;
				case OCI_TYPECODE_INTEGER:
					fld_list[n].fieldValue().setInt(m_define[n].intBuf);
					break;
                                    case DATA_TYPE_VARNUMBER : 
                                            g_oracle.OCINumberToInt( m_connect->errhp,
                                               &m_define[n].numBuf,
                                               8,
                                               OCI_NUMBER_SIGNED, 
                                               (dvoid*)&tmpnum);
					fld_list[n].fieldValue().setLLong(tmpnum);
                                            break;
				case DATA_TYPE_LONG: //long column ,m_describe[n].dbType = 8
				case DATA_TYPE_BINARY:
					fld_list[n].fieldValue().setBinary((char*)m_define[n].buf, m_define[n].colRetLen);
                                            break;
				case DATA_TYPE_LONGBINARY://long raw column
					pcLongColumn = fetchDefineFld();
					if(pcLongColumn)
					{
            					fld_list[n].fieldValue().setBinary(pcLongColumn, strlen(pcLongColumn));
						delete [] pcLongColumn;
					}
					break;
				default:
					fld_list[n].fieldValue().setString((char*)m_define[n].buf);
					break;
			}
		}
	}

	return 0;
}


//影响了多少条记录
int db_cursor::affectRec() const
{
	ub4			rowCount;
	ub4			sizep = sizeof(ub4);

	int status = g_oracle.OCIAttrGet(m_ociStmtp, OCI_HTYPE_STMT,&rowCount, &sizep,OCI_ATTR_ROW_COUNT, m_connect->errhp);
	
	return(rowCount);
}

db_cursorList::db_cursorList()
{

}

db_cursorList::~db_cursorList()
{
    free();
}
 

void db_cursorList::init()
{
	pHead = NULL;
	pTail = NULL;
}


int db_cursorList::add(int handle)
{
	db_cursor *ptr = new db_cursor;

	if(!pHead)
	{
		pHead = ptr;
		ptr->m_prev = NULL;
	}
	else
	{
		pTail->m_next = ptr;
		ptr->m_prev = pTail;
	}

	ptr->m_next = NULL;
	pTail = ptr;

	ptr->m_handle = handle;

	return 0;
}


db_cursor *db_cursorList::find(int handle)
{
	db_cursor *ptr = pHead;

	for(;ptr && ptr->m_handle!=handle;)
		ptr = ptr->m_next;

	return(ptr);
}

int db_cursorList::del(int handle)
{
	db_cursor *ptr;

	if((ptr=find(handle))==NULL)
		return(1);
	
	if(ptr==pHead) 
		pHead = pHead->m_next;
	else
		ptr->m_prev->m_next = ptr->m_next;

	if(ptr==pTail)
		pTail = pTail->m_prev;
	else
		ptr->m_next->m_prev = ptr->m_prev;

	delete ptr;

	return 0;
}

void  db_cursorList::free()
{
    db_cursor *curptr = pHead;
    db_cursor *nextptr = NULL;

    for(;curptr; curptr = nextptr)
    {
         nextptr = curptr->m_next;
         del(curptr->m_handle);
    }
}

oracleapi::oracleapi()
{
    phfree();
    m_curlist.init();
}


oracleapi::~oracleapi()
{
    m_curlist.free();
}

int oracleapi::getErr()
{
    return m_connect.getErr();
	
}

char* oracleapi::getErrInfo()
{
    return m_connect.getErrInfo();
}

int oracleapi::connect(char *user, char *password, char *hoststring)
{
    if(strlen(user) > 63 || strlen(password) > 127 || strlen(hoststring) > 63 )
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
    return m_connect.connect((char *)user, (char *)password, hoststring);
}

int oracleapi::checkConStatus(int reconInterval, int maxRetryTimess)
{
    int rtn;
    long iErrNo = 0;
    int try_times = 0;

    //不是重连错?

    iErrNo = m_connect.getErr();
    if(0==m_connect.isConnect() && (-99999 != iErrNo) )
    {
        return -1;
    }

    for (try_times = 0; try_times < maxRetryTimess; try_times++)
    {
        m_connect.setNoConnct();
        rtn = m_connect.connect(m_username, m_passwd, m_hostname);
        if(rtn >= 0)
        {
            return 1;
        }

        m_curlist.free();
        sleep(reconInterval);
    }

    return -99999;
}

int oracleapi::disconnect()
{
    m_username[0] = '\0';
    m_cursor.closeCur();
	return m_connect.disconnect();
}


void oracleapi::beginTrans()
{
	m_connect.beginTrans();
}


void oracleapi::commitTrans()
{	
	m_connect.commitTrans();
}


void oracleapi::rollbackTrans()
{
	m_connect.rollbackTrans();
}


int oracleapi::getRec(const char *sql, TFieldList& fld_list)
{
	int rtn;

	if(m_cursor.openCur(&m_connect)) 
		return -1;
	
	if(m_cursor.prepare(sql))
	{
		m_cursor.closeCur();
		return -1;
	}
	
	if(rtn = m_cursor.execute(SELECT_MODE))
	{
		m_cursor.closeCur();
		failover_transaction_cleanup();
		if(rtn == OCI_NO_DATA) return -3;
		else	return -1;
	}

	if(m_cursor.define()) 
	{
		m_cursor.closeCur();
		return -1;
	}

	if((rtn = m_cursor.fetch()) != 0 && rtn != OCI_NEED_DATA) 
	{
		m_cursor.closeCur();
		if(rtn == OCI_NO_DATA) return -3;
		else	return -1;
	}

	m_cursor.output(fld_list);
	m_cursor.closeCur();

	return 0;
}


//------------------------------------------------------------------------------
// sql Format ; "update table set col1='...', col2=:1" , 
void oracleapi::phfree()
{
    m_sql[0] = 0;
    for(int i=0; i<32; i++)
    {
        m_bin_fld_val_list[i].data = NULL;

    }

}


int oracleapi::inSetSql(const char *sql)
{
    m_numused = 0;
    m_cursor.closeCur();
	if(m_cursor.openCur(&m_connect)) 
	{
	    return -1;
	}
    
	if(m_cursor.prepare(sql))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}

	return 0;
}



int oracleapi::inExec(bool isCommit)
{
    m_numused = 0;	
    if(m_cursor.execute(NON_SELECT_MODE))
    {
        rollbackTrans();
        m_cursor.closeCur();
        return -1;
    }

   //added by alan --2009.6.3
    m_cursor.closeCur();
   /////////////////////////////

    if(isCommit == true)
    {
        commitTrans();
    }
    
    return 0;
}


int oracleapi::phSetSql(const char *sql)
{
    if(!sql)
    {
        return -1;
    }
    if(strlen(sql) >= MAX_DATA_BUF_LENGTH)
    {
        return -1;
    }
    
    phfree();    
    strncpy(m_sql, sql, MAX_DATA_BUF_LENGTH);
    return 0;
}


int oracleapi::phSetInt(unsigned int n, int *value)
{
    if(!value)
    {
        return -1;
    }
    if(n<=0 || n>32)
    {
        return -1;
    }

    m_bin_fld_val_list[n-1].type = SQL_INT;
    m_bin_fld_val_list[n-1].data = value;

    return 0;
}


int oracleapi::phSetLonglong(unsigned int n, long long *value)
{
    if(!value)
    {
        return -1;
    }
    if(n<=0 || n>32)
    {
        return -1;
    }

    m_bin_fld_val_list[n-1].type = SQL_LONG;
    m_bin_fld_val_list[n-1].data = value;

    return 0;
}


int oracleapi::phSetString(unsigned int n, char *value,int len)
{
    if(!value)
    {
        return -1;
    }
    if(n<=0 || n>32)
    {
        return -1;
    }

    m_bin_fld_val_list[n-1].type = SQL_STRING;
    m_bin_fld_val_list[n-1].data = value;

    return 0;
}


int oracleapi::phSetLongbyte(unsigned int n, void * value, int len)
{
    if(!value)
    {
        return -1;
    }
    if(n<=0 || n>32)
    {
        return -1;
    }

    m_bin_fld_val_list[n-1].type = SQL_BINARY;
    m_bin_fld_val_list[n-1].len = len;
    m_bin_fld_val_list[n-1].data = value;

    return 0;
}


int oracleapi::phExecute(bool isCommit)
{
    int rtn;

    if(m_connect.isConnect())
    {
        return -1;
    }
    
    rtn = inSetSql(m_sql);
    if(rtn < 0)
    {
        return -1;
    }

    for(int count = 0; count <32; count++)
    {
        if(!m_bin_fld_val_list[count].data)
            break;
        
        switch (m_bin_fld_val_list[count].type)
        {
            case SQL_INT:
                rtn = m_cursor.bindPos(count+1,(unsigned char *)m_bin_fld_val_list[count].data,sizeof(int),SQLT_INT,-1,(sb2 *)0);
                if(rtn < 0)
                {
                	m_cursor.closeCur();
                }
                break;
                
            case SQL_LONG:
                //added by alan,support for output parameters values from procedures.2009.08.04
                //OCINumberFromInt(m_connect.errhp, m_bin_fld_val_list[count].data, 8, OCI_NUMBER_SIGNED, &m_ocinum[m_numused]);    
                //rtn = m_cursor.bindPos(count+1,(unsigned char *)&m_ocinum[m_numused],sizeof(OCINumber), SQLT_VNU,-1,(sb2 *)0);
                rtn = m_cursor.bindPos(count+1,(unsigned char *)m_bin_fld_val_list[count].data,sizeof(long long), SQLT_INT,-1,(sb2 *)0);
	       if(rtn < 0)
                {
                    m_cursor.closeCur();
                }
                m_numused++;

                break;
                
            case SQL_STRING:
                rtn = m_cursor.bindPos(count+1,(unsigned char *)m_bin_fld_val_list[count].data,strlen((char *)m_bin_fld_val_list[count].data)+1,SQLT_CHR,-1,(sb2 *)0);
                if(rtn < 0)
                { 
                    m_cursor.closeCur();
                }
                break;

            case SQL_BINARY:
                rtn = m_cursor.bindPos(count+1,(unsigned char *)m_bin_fld_val_list[count].data,m_bin_fld_val_list[count].len,SQLT_BIN,-1,(sb2 *)0);
                if(rtn < 0)
                {
                    m_cursor.closeCur();
                }
                break;
                
            default:             
                rtn = -1;
                break;
                
        }
        if(rtn < 0)
        {
            return -1;
        }

    }

    rtn = inExec(isCommit);
    if(rtn < 0)
    {
        return -1;
    }
    
    phfree();    
    return 0;
}

int oracleapi::setLongFld(const char *sql,char *longMsg,int longType,bool isCommit)
{
	if(!longMsg) return 0;

	if(m_cursor.openCur(&m_connect)) 
		return -1;

	if(m_cursor.prepare(sql))
	{
		rollbackTrans();
		{
			m_cursor.closeCur();
			return -1;
		}
	}

	long iLength = strlen(longMsg);

	if(m_cursor.bindPos(1,(unsigned char *)longMsg,iLength,longType,-1,(sb2 *)0))
	{
		m_cursor.closeCur();
		return -1;
	}

	if(m_cursor.execute(NON_SELECT_MODE))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}

	m_cursor.closeCur();
	if(isCommit) commitTrans();

	return 0;
}


int oracleapi::query(const char *sql, int& handle)
{
	int	   hd, rtn;
	db_cursor *ptr=NULL;
	
	for(;;)
	{
		hd = rand();   
		ptr = m_curlist.find(hd);
		if(!ptr) break;
	}
	m_curlist.add(hd);
	ptr = m_curlist.find(hd);

	if(ptr->openCur(&m_connect))
	{
		m_curlist.del(hd);

		return -1;
	}

	if(ptr->prepare((char *)sql))
	{
		ptr->closeCur();
		m_curlist.del(hd);

		return -1;
	}

	if((rtn = ptr->execute(SELECT_MODE)))
	{
		ptr->closeCur();
		failover_transaction_cleanup();
		m_curlist.del(hd);

		return -1;
	}

	if(ptr->define())
	{
		ptr->closeCur();
		m_curlist.del(hd);

		return -1;
	}

	handle = hd;

	return 0;
}


int oracleapi::fetch(int handle, TFieldList &fld_list)
{
	db_cursor *ptr;

	ptr = m_curlist.find(handle);
	if(!ptr)
	{
		return(1);
	}

	int rtn;

	if((rtn = ptr->fetch()) != 0 && rtn != OCI_NEED_DATA)
	{
		if(rtn == OCI_NO_DATA) return -3;
		else	return -1;
	}

	ptr->output(fld_list);
	
	return 0;
}


int oracleapi::noValueFetch(int handle)
{

	db_cursor *ptr;

	ptr = m_curlist.find(handle);
	if(!ptr)
	{
		return 1;
	}

	int rtn;

	if((rtn = ptr->fetch()) != 0 && rtn != OCI_NEED_DATA )
	{
		if(rtn == OCI_NO_DATA) return -3;
		else	return -1;
	}

 	return 0;

}

//以后可能有用
//------------------------------------------------------------------------------
//get current record number for specify query handle
//------------------------------------------------------------------------------
int oracleapi::getCurRec(int handle, int *rowCount)
{
	db_cursor *ptr;

	ptr = m_curlist.find(handle);
	if(!ptr)
	{
		return 1;
	}

	*rowCount = ptr->affectRec();

	return 0;
}


int oracleapi::closequery(int handle)
{
	db_cursor *ptr;

	ptr = m_curlist.find(handle);
	if(!ptr)
	{
		return(1);
	}

	ptr->closeCur();

	m_curlist.del(handle);

	return 0;
}



int oracleapi::executeSql(const char *sql,int *iReturn, bool isCommit )
{
	if(m_cursor.openCur(&m_connect)) 
		return -1;
	
	if(m_cursor.prepare(sql))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}
	
	if(m_cursor.execute(NON_SELECT_MODE))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}

	*iReturn = m_cursor.affectRec();//effect affect record number

	m_cursor.closeCur();

	if(isCommit == true)	commitTrans();

	return 0;
}


int oracleapi::executeSql(const char *sql,bool isCommit)
{
	if(m_cursor.openCur(&m_connect)) 
		return -1;
	
	if(m_cursor.prepare(sql))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}
	
	if(m_cursor.execute(NON_SELECT_MODE))
	{
		rollbackTrans();
		m_cursor.closeCur();
		return -1;
	}

	m_cursor.closeCur();

	if(isCommit == true)	commitTrans();

	return 0;
}


bool oracleapi::isTable(const char *tab_name)
{
	char cSQL[512];
	sprintf(cSQL, "SELECT * FROM TAB WHERE TNAME='%s' AND TABTYPE='TABLE'",tab_name);

	if(m_cursor.openCur(&m_connect))
	{ 
		return(false);
	}
	
	if(m_cursor.prepare(cSQL))
	{
		m_cursor.closeCur();
		return(false);
	}
	
	if(m_cursor.execute(SELECT_MODE))
	{
		m_cursor.closeCur();
		failover_transaction_cleanup();
		return(false);
	}
	
	if(m_cursor.fetch() != 0)
	{
		m_cursor.closeCur();
		return(false);
	}

	m_cursor.closeCur();

	return(true);
}


int oracleapi::getSysDate(char *pcSysdate)
{
	if(!pcSysdate) return -1;

	TFieldList myList;
	int rtn = getRec((char *)"SELECT SYSDATE FROM DUAL", myList);
	if(rtn != 0) return(rtn);

	char *pcTmpSysdate = (char*)myList[0].fieldValue().asDate();

	if(pcTmpSysdate) strcpy(pcSysdate, pcTmpSysdate);

	return 0;
}


bool oracleapi::checkSql(const char *pcSQLStmt, bool isSelectSQL)
{
	if(!pcSQLStmt) return(false);

	if(m_cursor.openCur(&m_connect))
	{ 
		return(false);
	}
	

	if(m_cursor.prepare(pcSQLStmt))
	{
		m_cursor.closeCur();
		return(false);
	}

	if(isSelectSQL == true && m_cursor.execute(SELECT_MODE))
	{
		m_cursor.closeCur();
		failover_transaction_cleanup();
		return(false);
	}
	else if(isSelectSQL == false && m_cursor.execute(NON_SELECT_MODE))
	{
		m_cursor.closeCur();
		return(false);
	}

	m_cursor.closeCur();

	return(true);
}


int oracleapi::setDateFormat(DateFormat dateFormat)
{
	int rtn=-1;
	switch (dateFormat){
	case default_date :
		rtn = executeSql((char *)"Alter session set NLS_DATE_FORMAT = 'DD-MON-YY' ", false);
		break;
	case chinese_date:
		rtn = executeSql((char *)"Alter session set NLS_DATE_FORMAT = 'YYYY-MM-DD' ", false);
		break;
	case chinese_datetime:
		rtn = executeSql((char *)"Alter session set NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI:SS' ", false);
		break;
	default:
		rtn = executeSql((char *)"Alter session set NLS_DATE_FORMAT = 'DD-MON-YYYY' ", false);
		break;
	}

	return (rtn);
}

