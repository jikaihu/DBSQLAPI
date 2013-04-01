#ifndef __DBSQLAPIERROR_H__
#define __DBSQLAPIERROR_H__

#define LIB_ERR_NO_INVALID_FIELD_NUM 1
#define LIB_ERR_INFO_INVALID_FIELD_NUM "Invalid field number.[Field:%d, Num:%d]"

#define LIB_ERR_NO_INVALID_FIELD_NAME 2
#define LIB_ERR_INFO_INVALID_FIELD_NAME "The field name doesn't exist.[%s]"

#define LIB_ERR_MAX_FIELD_NUM 3
#define LIB_ERR_INFO_MAX_FIELD_NUM "The field number exceed max or less mix .Range:[%d,%d]"

#define LIB_ERR_STRING_IS_NULL 4
#define LIB_ERR_INFO_STRING_IS_NULL  "The input param is null"

#define LIB_ERR_UNKNOWN_DB_NAME 5
#define LIB_ERR_INFO_UNKNOWN_DB_NAME  "Unknow database name.[%s]"

#define LIB_ERR_DLOPEN_ERROR 6
#define LIB_ERR_INFO_DLOPEN_ERROR  "open shared library error.[%s]"

#define LIB_ERR_DLSYM_ERROR 7
#define LIB_ERR_INFO_DLSYM_ERROR  "load function(%s) error.[%s]"

#endif

