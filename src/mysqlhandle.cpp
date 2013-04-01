#include <stdio.h>
#include <stdlib.h>
#include "mysqlhandle.h"


const char* MYSQL_LIB = "libmysqlclient_r.so.16.0.0";        

mysqlHandle::mysqlHandle()
{
    pLibraryName = MYSQL_LIB;
    mysql_num_rows = NULL;
    mysql_num_fields = NULL;
    mysql_eof = NULL;
    mysql_fetch_field_direct = NULL;
    mysql_fetch_fields = NULL;
    mysql_row_tell = NULL;
    mysql_field_tell = NULL;
    mysql_field_count = NULL;
    mysql_affected_rows = NULL;
    mysql_insert_id = NULL;
    mysql_errno = NULL;
    mysql_error = NULL;
    mysql_info = NULL;
    mysql_thread_id = NULL;
    mysql_character_set_name = NULL;
    mysql_init = NULL;
    mysql_ssl_set = NULL;
    mysql_ssl_cipher = NULL;
    mysql_ssl_clear = NULL;
    mysql_connect = NULL;
    mysql_change_user = NULL;
    mysql_real_connect1 = NULL;
    mysql_real_connect2 = NULL;
    mysql_close = NULL;
    mysql_next_result = NULL;
    mysql_select_db = NULL;
    mysql_query = NULL;
    mysql_send_query = NULL;
    mysql_read_query_result = NULL;
    mysql_real_query = NULL;
    mysql_create_db = NULL;
    mysql_drop_db = NULL;
    mysql_shutdown = NULL;
    mysql_dump_debug_info = NULL;
    mysql_refresh = NULL;
    mysql_kill = NULL;
    mysql_ping = NULL;
    mysql_stat = NULL;
    mysql_get_server_info = NULL;
    mysql_get_client_info = NULL;
    mysql_get_host_info = NULL;
    mysql_get_proto_info = NULL;
    mysql_list_dbs = NULL;
    mysql_list_tables = NULL;
    mysql_list_fields = NULL;
    mysql_list_processes = NULL;
    mysql_store_result = NULL;
    mysql_use_result = NULL;
    mysql_options = NULL;
    mysql_free_result = NULL;
    mysql_data_seek = NULL;
    mysql_row_seek = NULL;
    mysql_field_seek = NULL;
    mysql_fetch_row = NULL;
    mysql_fetch_lengths = NULL;
    mysql_fetch_field = NULL;
    mysql_escape_string = NULL;
    mysql_real_escape_string = NULL;
    mysql_debug = NULL;
    mysql_odbc_escape_string = NULL;
    myodbc_remove_escape = NULL;
    mysql_thread_init = NULL;
    mysql_thread_end = NULL;
    mysql_thread_safe = NULL;
    mysql_server_end = NULL;
    mysql_set_character_set = NULL;
    // MySQL statement API functions
    mysql_stmt_init = NULL;
    mysql_stmt_prepare = NULL;
    mysql_stmt_execute = NULL;
    mysql_stmt_fetch = NULL;
    mysql_stmt_fetch_column = NULL;
    mysql_stmt_store_result = NULL;
    mysql_stmt_param_count = NULL;
    mysql_stmt_attr_set = NULL;
    mysql_stmt_attr_get = NULL;
    mysql_stmt_bind_param = NULL;
    mysql_stmt_bind_result = NULL;
    mysql_stmt_close = NULL;
    mysql_stmt_reset = NULL;
    mysql_stmt_free_result = NULL;
    mysql_stmt_send_long_data = NULL;
    mysql_stmt_result_metadata = NULL;
    mysql_stmt_param_metadata = NULL;
    mysql_stmt_errno = NULL;
    mysql_stmt_error = NULL;
    mysql_stmt_sqlstate = NULL;
    mysql_stmt_row_seek = NULL;
    mysql_stmt_row_tell = NULL;
    mysql_stmt_data_seek = NULL;
    mysql_stmt_num_rows = NULL;
    mysql_stmt_affected_rows = NULL;
    mysql_stmt_insert_id = NULL;
    mysql_stmt_field_count = NULL;

}

mysqlHandle::~mysqlHandle()
{

}

void mysqlHandle::loadDBFunc(void* dlHandle)
{
    loadFunc(dlHandle, (void*&)mysql_num_rows, "mysql_num_rows");
    loadFunc(dlHandle, (void*&)mysql_num_fields, "mysql_num_fields");
    loadFunc(dlHandle, (void*&)mysql_eof, "mysql_eof");
    loadFunc(dlHandle, (void*&)mysql_fetch_field_direct, "mysql_fetch_field_direct");
    loadFunc(dlHandle, (void*&)mysql_fetch_fields, "mysql_fetch_fields");
    loadFunc(dlHandle, (void*&)mysql_row_tell, "mysql_row_tell");
    loadFunc(dlHandle, (void*&)mysql_field_tell, "mysql_field_tell");
    loadFunc(dlHandle, (void*&)mysql_field_count, "mysql_field_count");
    loadFunc(dlHandle, (void*&)mysql_affected_rows, "mysql_affected_rows");
    loadFunc(dlHandle, (void*&)mysql_insert_id, "mysql_insert_id");
    loadFunc(dlHandle, (void*&)mysql_errno, "mysql_errno");
    loadFunc(dlHandle, (void*&)mysql_error, "mysql_error");
    loadFunc(dlHandle, (void*&)mysql_info, "mysql_info");
    loadFunc(dlHandle, (void*&)mysql_thread_id, "mysql_thread_id");
    loadFunc(dlHandle, (void*&)mysql_character_set_name, "mysql_character_set_name");
    loadFunc(dlHandle, (void*&)mysql_init, "mysql_init");
    loadFunc(dlHandle, (void*&)mysql_ssl_set, "mysql_ssl_set");
//    loadFunc(dlHandle, (void*&)mysql_ssl_cipher, "mysql_ssl_cipher");
//    loadFunc(dlHandle, (void*&)mysql_ssl_clear, "mysql_ssl_clear");
//    loadFunc(dlHandle, (void*&)mysql_connect, "mysql_connect");
    loadFunc(dlHandle, (void*&)mysql_change_user, "mysql_change_user");
//    loadFunc(dlHandle, (void*&)mysql_real_connect1, "mysql_real_connect1");
//    loadFunc(dlHandle, (void*&)mysql_real_connect2, "mysql_real_connect2");
    loadFunc(dlHandle, (void*&)mysql_close, "mysql_close");
    loadFunc(dlHandle, (void*&)mysql_next_result, "mysql_next_result");
    loadFunc(dlHandle, (void*&)mysql_select_db, "mysql_select_db");
    loadFunc(dlHandle, (void*&)mysql_query, "mysql_query");
    loadFunc(dlHandle, (void*&)mysql_send_query, "mysql_send_query");
    loadFunc(dlHandle, (void*&)mysql_read_query_result, "mysql_read_query_result");
    loadFunc(dlHandle, (void*&)mysql_real_query, "mysql_real_query");
//    loadFunc(dlHandle, (void*&)mysql_create_db, "mysql_create_db");
//    loadFunc(dlHandle, (void*&)mysql_drop_db, "mysql_drop_db");
    loadFunc(dlHandle, (void*&)mysql_shutdown, "mysql_shutdown");
    loadFunc(dlHandle, (void*&)mysql_dump_debug_info, "mysql_dump_debug_info");
    loadFunc(dlHandle, (void*&)mysql_refresh, "mysql_refresh");
    loadFunc(dlHandle, (void*&)mysql_kill, "mysql_kill");
    loadFunc(dlHandle, (void*&)mysql_ping, "mysql_ping");
    loadFunc(dlHandle, (void*&)mysql_stat, "mysql_stat");
    loadFunc(dlHandle, (void*&)mysql_get_server_info, "mysql_get_server_info");
    loadFunc(dlHandle, (void*&)mysql_get_client_info, "mysql_get_client_info");
    loadFunc(dlHandle, (void*&)mysql_get_host_info, "mysql_get_host_info");
    loadFunc(dlHandle, (void*&)mysql_get_proto_info, "mysql_get_proto_info");
    loadFunc(dlHandle, (void*&)mysql_list_dbs, "mysql_list_dbs");
    loadFunc(dlHandle, (void*&)mysql_list_tables, "mysql_list_tables");
    loadFunc(dlHandle, (void*&)mysql_list_fields, "mysql_list_fields");
    loadFunc(dlHandle, (void*&)mysql_list_processes, "mysql_list_processes");
    loadFunc(dlHandle, (void*&)mysql_store_result, "mysql_store_result");
    loadFunc(dlHandle, (void*&)mysql_use_result, "mysql_use_result");
    loadFunc(dlHandle, (void*&)mysql_options, "mysql_options");
    loadFunc(dlHandle, (void*&)mysql_free_result, "mysql_free_result");
    loadFunc(dlHandle, (void*&)mysql_data_seek, "mysql_data_seek");
    loadFunc(dlHandle, (void*&)mysql_row_seek, "mysql_row_seek");
    loadFunc(dlHandle, (void*&)mysql_field_seek, "mysql_field_seek");
    loadFunc(dlHandle, (void*&)mysql_fetch_row, "mysql_fetch_row");
    loadFunc(dlHandle, (void*&)mysql_fetch_lengths, "mysql_fetch_lengths");
    loadFunc(dlHandle, (void*&)mysql_fetch_field, "mysql_fetch_field");
    loadFunc(dlHandle, (void*&)mysql_escape_string, "mysql_escape_string");
    loadFunc(dlHandle, (void*&)mysql_real_escape_string, "mysql_real_escape_string");
    loadFunc(dlHandle, (void*&)mysql_debug, "mysql_debug");
//    loadFunc(dlHandle, (void*&)mysql_odbc_escape_string, "mysql_odbc_escape_string");
//    loadFunc(dlHandle, (void*&)myodbc_remove_escape, "myodbc_remove_escape");
    loadFunc(dlHandle, (void*&)mysql_thread_init, "mysql_thread_init");
    loadFunc(dlHandle, (void*&)mysql_thread_end, "mysql_thread_end");
    loadFunc(dlHandle, (void*&)mysql_thread_safe, "mysql_thread_safe");
    loadFunc(dlHandle, (void*&)mysql_server_end, "mysql_server_end");
    loadFunc(dlHandle, (void*&)mysql_set_character_set, "mysql_set_character_set");

    // MySQL statement API functions
    loadFunc(dlHandle, (void*&)mysql_stmt_init, "mysql_stmt_init");
    loadFunc(dlHandle, (void*&)mysql_stmt_prepare, "mysql_stmt_prepare");
    loadFunc(dlHandle, (void*&)mysql_stmt_execute, "mysql_stmt_execute");
    loadFunc(dlHandle, (void*&)mysql_stmt_fetch, "mysql_stmt_fetch");
    loadFunc(dlHandle, (void*&)mysql_stmt_fetch_column, "mysql_stmt_fetch_column");
    loadFunc(dlHandle, (void*&)mysql_stmt_store_result, "mysql_stmt_store_result");
    loadFunc(dlHandle, (void*&)mysql_stmt_param_count, "mysql_stmt_param_count");
    loadFunc(dlHandle, (void*&)mysql_stmt_attr_set, "mysql_stmt_attr_set");
    loadFunc(dlHandle, (void*&)mysql_stmt_attr_get, "mysql_stmt_attr_get");
    loadFunc(dlHandle, (void*&)mysql_stmt_bind_param, "mysql_stmt_bind_param");
    loadFunc(dlHandle, (void*&)mysql_stmt_bind_result, "mysql_stmt_bind_result");
    loadFunc(dlHandle, (void*&)mysql_stmt_close, "mysql_stmt_close");
    loadFunc(dlHandle, (void*&)mysql_stmt_reset, "mysql_stmt_reset");
    loadFunc(dlHandle, (void*&)mysql_stmt_free_result, "mysql_stmt_free_result");
    loadFunc(dlHandle, (void*&)mysql_stmt_send_long_data, "mysql_stmt_send_long_data");
    loadFunc(dlHandle, (void*&)mysql_stmt_result_metadata, "mysql_stmt_result_metadata");
    loadFunc(dlHandle, (void*&)mysql_stmt_param_metadata, "mysql_stmt_param_metadata");
    loadFunc(dlHandle, (void*&)mysql_stmt_errno, "mysql_stmt_errno");
    loadFunc(dlHandle, (void*&)mysql_stmt_error, "mysql_stmt_error");
    loadFunc(dlHandle, (void*&)mysql_stmt_sqlstate, "mysql_stmt_sqlstate");
    loadFunc(dlHandle, (void*&)mysql_stmt_row_seek, "mysql_stmt_row_seek");
    loadFunc(dlHandle, (void*&)mysql_stmt_row_tell, "mysql_stmt_row_tell");
    loadFunc(dlHandle, (void*&)mysql_stmt_data_seek, "mysql_stmt_data_seek");
    loadFunc(dlHandle, (void*&)mysql_stmt_num_rows, "mysql_stmt_num_rows");
    loadFunc(dlHandle, (void*&)mysql_stmt_affected_rows, "mysql_stmt_affected_rows");
    loadFunc(dlHandle, (void*&)mysql_stmt_insert_id, "mysql_stmt_insert_id");
    loadFunc(dlHandle, (void*&)mysql_stmt_field_count, "mysql_stmt_field_count");

    loadFunc(dlHandle, (void*&)mysql_commit, "mysql_commit");
    loadFunc(dlHandle, (void*&)mysql_server_init, "mysql_server_init");
    loadFunc(dlHandle, (void*&)mysql_real_connect, "mysql_real_connect");
    loadFunc(dlHandle, (void*&)mysql_rollback, "mysql_rollback");

}


