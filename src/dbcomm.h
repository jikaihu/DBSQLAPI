#ifndef H_DBCOMM_H
#define H_DBCOMM_H

enum  DateFormat{
	default_date,	//dd-mon-yy
	chinese_date,	//YYYY-MM-DD
	chinese_datetime	//YYYY-MM-DD HH24:MI:SS
};

typedef enum
{
    SQL_No_Error,
    SQL_Library_Error,
    SQL_DBMS_API_Error,
    SQL_Error_Reserved = (int)(((unsigned int)(-1))/2)
} ErrorType;



enum DataType
{
    SQL_UNKNOWN = 0,
    SQL_NULL,
    SQL_INT,
    SQL_UINT,
    SQL_LONG,
    SQL_ULONG,
    SQL_FLOAT,
    SQL_DOUBLE,
    SQL_DATE,
    SQL_STRING,
    SQL_BINARY,
    SQL_TEXT
};

#define	MAX_DATA_BUF_LENGTH    2048

#endif
