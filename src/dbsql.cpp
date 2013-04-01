#include "dbsql.h"
#include "oracleapi.h"
#include "mysqlapi.h"
#include "dbapi.h"
#include "dbsqlapierror.h"

const char* g_version = "dbsql api version %s (built %s %s)";

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


const char* TValue::toString(char* sBuf)
{
    char* p = sBuf;
    switch(eDataType)
    {   
        case SQL_NULL:
            p = "NULL";
            break;
        case SQL_INT:
            if(p)
                sprintf(p, "%d", uValue.i);
            break;
        case SQL_UINT:
            if(p)
                sprintf(p, "%u", uValue.ui);
            break;
        case SQL_LONG:
            if(p)
                sprintf(p, "%lld", uValue.l);
            break;
        case SQL_ULONG:
            if(p)
                sprintf(p, "%llu", uValue.ul);
            break;
        case SQL_FLOAT:
            if(p)
                sprintf(p, "%f", uValue.f);
            break;
        case SQL_DOUBLE:
            if(p)
                sprintf(p, "%f", uValue.d);
            break;
        case SQL_DATE:
        case SQL_STRING:
        case SQL_TEXT:
            p = uValue.p;
            break;
        case SQL_BINARY:
            p = uValue.p;
            break;
        default:
            p = "";
            break;
    }
    return p;
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
    bufferRead(sStr, strlen(sStr)+1, true);
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

void TValue::bufferRead(const char* sData, int iLen, bool isString)
{
    if (!sData) 
        return;
    
    freeBuffer();
    
    uValue.p = (char *)new char[iLen];
    if (!uValue.p) 
        return;
    if(isString)
    {
        strcpy(uValue.p, sData);
    }
    else
    {
        memcpy(uValue.p, sData, iLen);
    }
        

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
        throw TException(LIB_ERR_STRING_IS_NULL, LIB_ERR_INFO_STRING_IS_NULL);
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
    {
        throw TException(LIB_ERR_MAX_FIELD_NUM, LIB_ERR_INFO_MAX_FIELD_NUM,
            1, MAXITEMS);
    }
    freeFldList();
    num = iFieldNum;
    fld_list = (TField *) new TField[num];
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
        throw TException(LIB_ERR_NO_INVALID_FIELD_NUM, LIB_ERR_INFO_INVALID_FIELD_NUM,
            num, nField);
    }
    return fld_list[nField];
}

TField& TFieldList::operator[](const char* sFieldName)
{
    int iFieldPos = 0;
    for(int iFieldPos = 0; iFieldPos < num; iFieldPos++)
    {
        if(0 == strcmp(fld_list[iFieldPos].getFieldName(), sFieldName))
        {
            break;
        }
    }
    if(iFieldPos == num)
    {
        throw TException(LIB_ERR_NO_INVALID_FIELD_NAME, LIB_ERR_INFO_INVALID_FIELD_NAME,
            sFieldName);
    }
    return fld_list[iFieldPos];
}


void TFieldList::printFldList()
{
    unsigned int i;
    char *str;
    char sBufTmp[32];
    for(i=0; i<num; i++)
    {
        if (fld_list[i].fieldValue().isNULL())
        {
            printf("field: %s  value: \n", fld_list[i].getFieldName());
        }
        else
        {
            printf("field: %s  value: %s\n", fld_list[i].getFieldName(), fld_list[i].fieldValue().toString((char*)sBufTmp));
        }

    }
    return;
}

TException::TException()
{
    sql_error_type = SQL_No_Error;
    sql_err_no = 0;
    native_err_no = 0;
    sql_err_info[0] = 0;
}

TException::TException(const TException &other)
{
    sql_error_type = other.sql_error_type;
    sql_err_no = other.native_err_no;
    native_err_no = other.native_err_no;
    strcpy(sql_err_info, other.sql_err_info);
}


TException::TException(int iErrNo, const char* sErrMsg, ...)
{
    va_list ap;
    va_start(ap, sErrMsg);
    vsprintf(sql_err_info, sErrMsg, ap);
    va_end(ap);
    sql_err_no = iErrNo;
    sql_error_type = SQL_Library_Error;
    native_err_no = 0;
}

TException::TException(int iNativeErrNo, int iErrNo, const char* sErrMsg, ...)
{
    va_list ap;
    va_start(ap, sErrMsg);
    vsprintf(sql_err_info, sErrMsg, ap);
    va_end(ap);
    sql_err_no = iErrNo;
    sql_error_type = SQL_DBMS_API_Error;
    native_err_no = iNativeErrNo;
}


int TException::getErrNo()
{
    return sql_err_no;
}

int TException::getNativeErrNo()
{
    return native_err_no;
}

const char* TException::getErrInfo()
{
    return sql_err_info;
}

TException::~TException()
{

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
    if(!db_name)
    {
        throw TException(LIB_ERR_STRING_IS_NULL, LIB_ERR_INFO_STRING_IS_NULL);
    }
    pdb = NULL;
    if(strcmp(db_name,"oracle") == 0)
    {
        initDB(Oracle_Client);
    }
    else if(strcmp(db_name,"mysql") == 0)
    {
        initDB(Mysql_Client);
    }
    else
    {
        throw TException(LIB_ERR_UNKNOWN_DB_NAME, LIB_ERR_INFO_UNKNOWN_DB_NAME, db_name);
    }
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
    pdb->phSetSql(sql); 
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


void dbsql::closeQuery(int handle)
{
    pdb->closeQuery(handle);
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



const char* TGlobals::getVersion()
{
    static char dbsqlapiVer[256];

    sprintf(dbsqlapiVer, g_version, SQL_VERSION, __DATE__, __TIME__);

    return  dbsqlapiVer;
}


