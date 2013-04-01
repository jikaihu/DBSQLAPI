#include <stdio.h>
#include <unistd.h>
#include <dbsql.h>
 #include <sys/time.h>
// g++ -o dbsqlut dbsqlut.cpp -I../src-L.. -ldbsql

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        printf("Invalid param.\n");
        return -1;
    }
    try
    {
    int ret = 0;
    //dbsql con(Oracle_Client);
    dbsql con(Mysql_Client);
    //con.connect("hitv", "hitv2009", "hitv132");
    con.connect("hitv", "hitv", "10.0.64.13");

    char* sql;
    //char* sql = "create table dbwrite(id int primary key, name varchar2(400))";
    //ret = con.execute_sql(sql, true);

    timeval beginTime,endTime;
    timeval totalbeginTime,totalendTime;
    char sqlbuf[1024];
    int chose = atoi(argv[1]);
    int count = atoi(argv[2]);
  //  count = 10000;
  //  chose = 0;
  //  while(true)
  //  {
  //      chose = (chose++)%5;
  //      chose = (chose==0)?1:chose;
    switch(chose)
    {
        case 1:
            {
                sql = "delete from SHINEFS";
                con.executeSql(sql, true);
                gettimeofday(&totalbeginTime, NULL);    
                for(int i = 0; i < count; i++)
                {
                    sprintf(sqlbuf, "insert into SHINEFS(shinefsseq, ip, port, LOAD_INCOMING, LOAD_OUTGOING, TIMESTAMP, BAND_IN, BAND_OUT, DOMAINID, BEHIND_NAT, SESSION_SPEED_IN, SESSION_SPEED_OUT) values(%d, 100,9002,1,1,2222,1,1,77,0,100,100)", i);
                    gettimeofday(&beginTime, NULL);    
                    con.executeSql(sqlbuf, true);
                    if(ret < 0)
                    {
                        printf("Execute sql failed, ErrorInfo:%s\n", con.getErrInfo());
                        //return -1;
                    }
                    gettimeofday(&endTime, NULL);    
                    double d = (endTime.tv_sec - beginTime.tv_sec) + (double)(endTime.tv_usec - beginTime.tv_usec)/double(1000*1000);
                    if(d > 1)
                    {
                        printf("Time:%.6f(s)\n", d);
                        break;
                    }
                    if((i%500) == 0) {
                        sleep(1);
                    }
                }
                gettimeofday(&totalendTime, NULL);  
                double d = (totalendTime.tv_sec - totalbeginTime.tv_sec) + (double)(totalendTime.tv_usec - totalbeginTime.tv_usec)/double(1000*1000);
                printf("Time:%.6f(s)\n", d);
            }
            break;
        case 2:
            {
                gettimeofday(&totalbeginTime, NULL);    
                for(int i = 0; i < count; i++)
                {
                    sprintf(sqlbuf, "update SHINEFS set ip=12121 where shinefsseq=%d", i);
                    gettimeofday(&beginTime, NULL);    
                    con.executeSql(sqlbuf, true);
                    if(ret < 0)
                    {
                        printf("Execute sql failed, ErrorInfo:%s\n", con.getErrInfo());
                        //return -1;
                    }
                    gettimeofday(&endTime, NULL);    
                    double d = (endTime.tv_sec - beginTime.tv_sec) + (double)(endTime.tv_usec - beginTime.tv_usec)/double(1000*1000);
                    if(d > 1)
                    {
                        printf("Time:%.6f(s)\n", d);
                        break;
                    }
                    if((i%500) == 0) {
                        sleep(1);
                    }
                }
                gettimeofday(&totalendTime, NULL);  
                double d = (totalendTime.tv_sec - totalbeginTime.tv_sec) + (double)(totalendTime.tv_usec - totalbeginTime.tv_usec)/double(1000*1000);
                printf("Time:%.6f(s)\n", d);
            }
            break;
        case 3:
            {
                gettimeofday(&totalbeginTime, NULL);    
                for(int i = 0; i < count; i++)
                {
                    sprintf(sqlbuf, "delete from SHINEFS where shinefsseq=%d", i);
                    gettimeofday(&beginTime, NULL);    
                    con.executeSql(sqlbuf, true);
                    if(ret < 0)
                    {
                        printf("Execute sql failed, ErrorInfo:%s\n", con.getErrInfo());
                        //return -1;
                    }
                    gettimeofday(&endTime, NULL);    
                    double d = (endTime.tv_sec - beginTime.tv_sec) + (double)(endTime.tv_usec - beginTime.tv_usec)/double(1000*1000);
                    if(d > 1)
                    {
                        printf("Time:%.6f(s)\n", d);
                        break;
                    }
                    if((i%500) == 0) {
                        sleep(1);
                    }
                }
                gettimeofday(&totalendTime, NULL);  
                double d = (totalendTime.tv_sec - totalbeginTime.tv_sec) + (double)(totalendTime.tv_usec - totalbeginTime.tv_usec)/double(1000*1000);
                printf("Time:%.6f(s)\n", d);
            }
            break;
        case 4:
            {
                gettimeofday(&totalbeginTime, NULL);    
                for(int i = 0; i < count; i++)
                {
                    gettimeofday(&beginTime, NULL);    
                    sprintf(sqlbuf, "%s", "select * from SHINEFS");
                    //sprintf(sqlbuf, "%s", "select * from subscriber");
                    TFieldList tFldList;
                    int iSqlHandle = 0;
                    con.query(sqlbuf, iSqlHandle);
                    if(ret < 0)
                    {
                        printf("query error.%d,%s\n", con.getErr(), con.getErrInfo());
                        return -1;
                    }
                    int count = 0;
                    while(1)
                    {
                        ret = con.fetch(iSqlHandle, tFldList);
                        if(ret < 0)
                        {
                            printf("return:%d\n", ret);
                            break;
                        }
                        tFldList.printFldList();
                        //print_fldList(&tFldList);
                        count++;
                    }
                    con.closeQuery(iSqlHandle);
                   // printf("Count:%d\n", count);
                }
                gettimeofday(&totalendTime, NULL);  
                double d = (totalendTime.tv_sec - totalbeginTime.tv_sec) + (double)(totalendTime.tv_usec - totalbeginTime.tv_usec)/double(1000*1000);
                printf("Time:%.6f(s)\n", d);
            }
            break;
        case 5:
            {
                gettimeofday(&totalbeginTime, NULL);    
                for(int i = 0; i < count; i++)
                {
                    gettimeofday(&beginTime, NULL);    
                    //sprintf(sqlbuf, "%s", "insert into SHINEFS(SHINEFSSEQ, IP) values(:p1,:p2)");
                    sprintf(sqlbuf, "%s", "insert into t values(:p1)");
                    int shinefsseq = 100;
                    int shinefsip = -100;
                    con.phSetSql(sqlbuf);
                    con.phSetInt(1, &shinefsip);
                   // ret = con.phSet_Int(2, &shinefsip);
                    con.phExecute(true);
                }
                gettimeofday(&totalendTime, NULL);  
                double d = (totalendTime.tv_sec - totalbeginTime.tv_sec) + (double)(totalendTime.tv_usec - totalbeginTime.tv_usec)/double(1000*1000);
                printf("Time:%.6f(s)\n", d);
            }
            break;
            
        default:
            {
                printf("Invalid param.\n");
            }
            break;
    }
    //}
    con.disconnect();
    }
    catch(TException& ex)
    {
        printf("%s\n", ex.getErrInfo());
    }
    return 1;
}


