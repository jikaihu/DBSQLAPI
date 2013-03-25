#include "dbsql.h"
#include "oracleapi.h"
#include "mysqlapi.h"
#include "dbapi.h"

TValue::TValue()
{
    eDataType = SQL_UNKNOWN;
    uValue.p = NULL;
    valueLen = 0;
}

TValue::TValue(const TValue& other)
{
    *this = other;
}

TValue &TValue::operator = (const TValue& other)
{
    if(this == &other)
    {
        return *this;
    }
    eDataType = other.eDataType;
    valueLen = other.valueLen;
    switch(eDataType)
    {
        case SQL_UNKNOWN:
        case SQL_STRING:
        case SQL_DATE:
        case SQL_BINARY:
        case SQL_TEXT:
            bufferRead(other.uValue.p, valueLen);
            break;
        default:
            uValue.l = other.uValue.l;
            break;
    }
    return *this;
}

TValue:: ~TValue()
{
    freeBuffer();
}

int TValue::asInt()
{
    return uValue.i;
}

unsigned int TValue::asUInt()
{
    return uValue.ui;
}

long long TValue::asLLong()
{
    return uValue.l;
}

unsigned long long TValue::asULong()
{
    return uValue.ul;
}


const char* TValue::asString()
{
    return uValue.p;
}


double TValue::asDouble()
{
    return uValue.d;
}

float TValue::asFloat()
{
    return uValue.f;
}

const char* TValue::asDate()
{
    return uValue.p;
}

const void* TValue::asBinary()
{
    return uValue.p;
}

int TValue::getLength()
{
    return valueLen;
}


DataType TValue::getType()
{
    return eDataType;
}

bool TValue::isNULL()
{
    if(eDataType == SQL_NULL)
    {
        return true;
    }
    return false;
}

void TValue::setInt(int iValue)
{
    uValue.i = iValue;
    eDataType = SQL_INT;
}


void TValue::setUInt(unsigned int iValue)
{
    uValue.ui = iValue;
    eDataType = SQL_UINT;
}


void TValue::setFloat(float fValue)
{
    uValue.f = fValue;
    eDataType = SQL_FLOAT;
}

void TValue::setDouble(double dValue)
{
    uValue.d = dValue;
    eDataType = SQL_DOUBLE;
}

void TValue::setLLong(long long lValue)
{
    uValue.l = lValue;
    eDataType = SQL_LONG;
}

void TValue::setULong(unsigned long long lValue)
{
    uValue.ul = lValue;
    eDataType = SQL_ULONG;
}
void TValue::setDate(const char* sDate)
{
    bufferRead(sDate, strlen(sDate));
    eDataType = SQL_DATE;
}

void TValue::setString(const char* sStr)
{
    bufferRead(sStr, strlen(sStr));
    eDataType = SQL_STRING;
}

void TValue::setBinary(const char* sData, int iLen)
{
    bufferRead(sData, iLen);
    eDataType = SQL_BINARY;
}

void TValue::setValue(const char* sData, int iLen)
{
    bufferRead(sData, iLen);
    eDataType = SQL_UNKNOWN;
}

void TValue::setNULL()
{
    eDataType = SQL_NULL;
}

void TValue::bufferRead(const char* sData, int iLen)
{
    if (!sData) 
        return;
    
    freeBuffer();
    
    uValue.p = (char *)new char[iLen];
    if (!uValue.p) 
        return;

    memcpy(uValue.p, sData, iLen);

}

void TValue::freeBuffer()
{
    if(eDataType == SQL_BINARY || eDataType == SQL_DATE ||
        eDataType == SQL_STRING || eDataType == SQL_UNKNOWN)
    {
        if(uValue.p)
        {
            delete [] uValue.p;
            uValue.p = NULL;
            eDataType == SQL_UNKNOWN;
        }
    }
}


TField::TField()
{
    fieldName[0] = 0;
    op = 0;
    iSourceType = 0;
}

TField::~TField()
{

}

void TField::setFieldName(const char* sName)
{
    if(NULL == sName)
    {
        return ;
    }
    strncpy(fieldName, sName, MAX_FIELD_NAME_LEN - 1);
    fieldName[MAX_FIELD_NAME_LEN - 1] = 0;
}

void TField::setSourceType(int iType)
{
    iSourceType = iType;
}


TValue& TField::fieldValue()
{
    return value;
}

int TField::getOp()
{
    return op;
}

const char* TField::getFieldName()
{
    return fieldName;
}

int TField::getSourceType()
{
    return iSourceType;
}


void TField::setFieldValue(TValue& value)
{
    value = value;
}

void TField::setOp(int iOp)
{
    op = iOp;
}

TFieldList::TFieldList():num(0),fld_list(NULL)
{

}

TFieldList::~TFieldList()
{
    freeFldList();
}


void TFieldList::newFldList(int iFieldNum)
{
    if((iFieldNum > MAXITEMS) || (iFieldNum < 1))
        return;
    freeFldList();
    num = iFieldNum;
    fld_list = (TField *) new TField[num];

    if(!fld_list) return;

    return;
}


void TFieldList::freeFldList()
{
    if(fld_list)
    {
        delete [] fld_list;
        fld_list = NULL;
        num = 0;
    }

    return;
}

TField& TFieldList::operator[](unsigned int nField)
{
    if(nField >= num)
    {
        throw TException(9999, "Invalid Field number.");
    }
    return fld_list[nField];
}

void TFieldList::printFldList()
{
    unsigned int i;
    char *str;

    for(i=0; i<num; i++)
    {
        if (fld_list[i].fieldValue().isNULL())
        {
            printf("field: %s  value: \n", fld_list[i].getFieldName());
        }
        else
        {
            printf("field: %s  value: %s\n", fld_list[i].getFieldName(), fld_list[i].fieldValue().toString());
        }

    }
    return;
}


dbsql::dbsql()
{
    pdb = NULL;
    used_db = Unknown;
}

dbsql::~dbsql()
{
    if(pdb)
	delete pdb;
}

void dbsql::initDB(DB_Client edb)
{
    used_db = edb; 
    if(!pdb)
    {
        switch(used_db)
        {
            case Oracle_Client:
                pdb = new oracleapi;
                break;;
            case Mysql_Client:
                pdb = new mysqlapi;
                break;
            default:
                break;
        }
    }
    return;
}



dbsql::dbsql(DB_Client edb)
{
	pdb = NULL;
	initDB(edb);
}

dbsql::dbsql(const char * db_name)
{
    pdb = NULL;
    if(strcmp(db_name,"oracle") == 0)
        initDB(Oracle_Client);
    if(strcmp(db_name,"mysql") == 0)
        initDB(Mysql_Client);
	
}


int dbsql::getErr()
{
   pdb->getErr();
}

char* dbsql::getErrInfo()
{
   return pdb->getErrInfo();
}

void dbsql::connect(char *user, char *password, char *hoststring)
{
    pdb->connect(user,password,hoststring);
}

int dbsql::checkConStatus(int reconInterval, int maxRetryTimess)
{
    return pdb->checkConStatus(reconInterval,maxRetryTimess);
}

void dbsql::disconnect()
{
    pdb->disconnect();
}


void dbsql::beginTrans()
{
    pdb->beginTrans();
}


void dbsql::commitTrans()
{	
    pdb->commitTrans();
}


void dbsql::rollbackTrans()
{
    pdb->rollbackTrans();
}


int dbsql::getRec(const char *sql, TFieldList& fld_list)
{
    pdb->getRec(sql,fld_list);
}


void dbsql::phfree()
{
    pdb->phfree();
}


void dbsql::phSetSql(const char *sql)
{
   char temp_sql[MAX_DATA_BUF_LENGTH];
   memset(temp_sql,0,MAX_DATA_BUF_LENGTH);
   int cp_flag=1;
   if (used_db == Oracle_Client) 
		pdb->phSetSql(sql); 
   if(used_db == Mysql_Client) 
   {
   	if(strlen(sql) >= MAX_DATA_BUF_LENGTH)
		return;
	int offset=0;
   	for(int i=0;i<strlen(sql);i++)
   	{
		if(sql[i] == ':')
		{
			temp_sql[offset]='?';
			offset ++;
			cp_flag=0;
			//continue;
		}
		if((sql[i] == ','  || sql[i] == ' ' ||sql[i] == ')'  ||sql[i] == ';' ) && cp_flag == 0)
			cp_flag=1;
		if(cp_flag == 1)
		{
			temp_sql[offset]=sql[i];
			offset ++;
		}		
		if(i == strlen(sql)-1)
			temp_sql[offset+1] = '\0';
		#ifdef DEBUG
			printf("************************************************\n");
			printf("sql[%d]:%c\ntemp_sql[%d]:%c\n",i,sql[i],i,temp_sql[offset]);
		#endif
   	}
	#ifdef DEBUG
	       fprintf(stderr,"sql:%s\ntemp_sql:%s\n",sql,temp_sql);
	#endif
	pdb->phSetSql(temp_sql); 
   }
}


void dbsql::phSetInt(unsigned int n, int *value)
{
    pdb->phSetInt(n,value);
}


void dbsql::phSetLonglong(unsigned int n, long long *value)
{
    pdb->phSetLonglong(n,value);
}


void dbsql::phSetString(unsigned int n, char *value,int len)
{
    pdb->phSetString(n,value,len);
}


void dbsql::phSetLongbyte(unsigned int n, void * value, int len)
{
   pdb->phSetLongbyte(n,value,len);
}


void dbsql::phExecute(bool isCommit)
{
    pdb->phExecute(isCommit);
}

void dbsql::setLongFld(const char *sql,char *longMsg,int longType,bool isCommit)
{
    pdb->setLongFld(sql,longMsg,longType,isCommit);
}


void dbsql::query(const char *sql, int& handle)
{
    pdb->query(sql,handle);
}


int dbsql::fetch(int handle,TFieldList& fld_list)
{
    return pdb->fetch(handle,fld_list);
}


void dbsql::noValueFetch(int handle)
{
    pdb->noValueFetch(handle);
}

void dbsql::getCurRec(int handle, int *rowCount)
{
    pdb->getCurRec(handle,rowCount);
}


void dbsql::closequery(int handle)
{
    pdb->closequery(handle);
}



void dbsql::executeSql(const char *sql,int *iReturn, bool isCommit )
{
    pdb->executeSql(sql,iReturn,isCommit);
}


void dbsql::executeSql(const char *sql,bool isCommit)
{
    pdb->executeSql(sql,isCommit);
}


bool dbsql::isTable(const char *tab_name)
{
    return pdb->isTable(tab_name);
}


void dbsql::getSysDate(char *pcSysdate)
{
    pdb->getSysDate(pcSysdate);
}


bool dbsql::checkSql(const char *pcSQLStmt, bool isSelectSQL)
{
    return pdb->checkSql(pcSQLStmt,isSelectSQL);
}


void dbsql::setDateFormat(DateFormat dateFormat)
{
    pdb->setDateFormat(dateFormat);
}


void   dbsql::changeDB(const char* db_name)
{
    pdb->changeDB(db_name);
}

