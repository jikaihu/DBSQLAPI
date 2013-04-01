#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "dbapi.h"

int   main()     
{    
  int ret=0;
  char   *user   =   "hitv",   *pwd   =   "hitv",   *dbname   =   "localhost";     
  //char   *user   =   "hitv",   *pwd   =   "hitv2009",   *dbname   =   "hitv"; 
 //char result[64];
  //dbapi api(Oracle_Client);
 //dbapi api(Mysql_Client);
  dbapi api;
  //api.initDB(Mysql_Client);

  if((ret=api.connect(user, pwd, dbname))<0)
  {
  	printf("ret=%d\tError MSG :%s!\n",ret,api.getErrInfo());
//	return -1;
  }
  switch(api.get_db_source())
  {
  	case Unknown:
		printf("Connect DB Source is Unknown!\n");
		break;
  	case Mysql_Client:
  		printf("Connect DB Source is Mysql_Client!\n");
		break;
	case Oracle_Client:
		printf("Connect DB Source is Oracle_Client!\n");
		break;
	default:
		break;
  }
  
  /*
   api.getSysDate(result);
   printf("api.getSysDate : %s\n",result);
*/
/*
	if(api.setDateFormat(default_date)<0)
		printf("err[%d]:%s\n",api.getErr(),api.getErrInfo());

*/		
         TFldList my_list;
	memset(&my_list,0,sizeof(TFldList));
	if((ret=api.getRec("select * from jzh", &my_list))<0)
		printf("ret:%d\terr:%s\n",ret,api.getErrInfo());
	print_fldList(&my_list);
	free_fldList(&my_list);

/*
	int hd,row_count=0;
	TFldList my_list;	
	memset(&my_list,0,sizeof(TFldList));
	if(api.query("select * from jzh", &hd)<0)
	{
		printf("err:%s\n",api.getErrInfo());
		return -1;
	}
	
	while(api.fetch(hd, &my_list) == 0)
	//while(api.noValueFetch(hd) == 0)
	{
		print_fldList(&my_list);
		free_fldList(&my_list);
		api.getCurRec(hd, &row_count);
		printf("Handle[%d]row_count : %d\n",hd,row_count);
	}

	api.closeQuery(hd);
  
  char * sql ="UPDATE USERCAPABILITY SET  DEVID='86100200100052100000000300000158', OVERLAYADDR='STB84767@200.1', IPADDRESS='172.16.27.253', NETMASK='255.255.255.0', DEVTYPE=1, DEVVERSION='0.0.0.0', LANCAPABILITY='c0000000000000000000000000000000', DEVCAPABILITY='4040e0800001f4005e00780000000000640000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', REGISTERTIME=1255598554 where SUBSCRIBERID=84767";
  if(api.executeSql(sql, true)<0)
  {
		printf("err:%s\n",api.getErrInfo());
		return -1;
   }	
 */ 
  //api.phSetSql("INSERT  INTO IOTEST (SUBSCRIBERID, BEHAVIORTYPE, BEHAVIOR, OBJECTID) VALUES(:p1, :p2, :p3, :p4)");
  //api.phSetSql("ahrg:iweii vnsd:pp :ee)djfk-:kkk;");
  //printf("Error MSG :%s!\n",api.getErrInfo());
 api.disconnect();

 exit(0);
}     


