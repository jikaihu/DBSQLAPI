CPP = g++
CC = gcc

TOP_DIR = ..

DBSQLOBJ = ./dbsql.o
VEROBJ=./ver.o
ORACLEAPIOBJ=./oracleapi.o
MYSQLAPIOBJ=./mysqlapi.o
NVLISTOBJ=./nvlist.o
DBPUBOBJ=./dbpub.o
DBHANDLE=./dbhandle.o
MYSQLHANDLE=./mysqlhandle.o
ORACLEHANDLE=./orahandle.o
DBAPI=./dbapi.o

OBJS=${DBSQLOBJ} $(ORACLEAPIOBJ) $(MYSQLAPIOBJ) $(VAROBJ) $(NVLISTOBJ) ${DBPUBOBJ} ${DBHANDLE} ${MYSQLHANDLE} ${ORACLEHANDLE} ${DBAPI}

TARGET=${TOP_DIR}/libdbsql.so


LIBS = -L/usr/lib64/mysql -lmysqlclient_r -fPIC -dl
#LIBS = `mysql_config --libs_r` -lmysqlclient_r

INCS = -I./  -I${TOP_DIR}/rdb-inc/oracle -I/usr/include/mysql 
#INCS = -I./  -I${TOP_DIR}/rdb-inc/oracle  `mysql_config --cflags` 


CXXFLAGS = -g -fpic -DSQL_VERSION=\"$(VERSION)\" -DHAVE_DLOPEN -DHAVE_DLFCN_H
CFLAGS =
SOFLAGS = -g -shared -Wall
OFLAGS = -g -Wall

all : $(TARGET) 

$(TARGET):$(OBJS)
	$(CPP) $(SOFLAGS) $^ -o $@ $(LIBS) 

$(ORACLEAPIOBJ):$(TOP_DIR)/src/oracleapi.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(MYSQLAPIOBJ):$(TOP_DIR)/src/mysqlapi.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(DBSQLOBJ):$(TOP_DIR)/src/dbsql.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(VEROBJ): ${TOP_DIR}/src/ver.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)
	
$(NVLISTOBJ): ${TOP_DIR}/src/namevalue.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(DBPUBOBJ): ${TOP_DIR}/src/dbpub.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(DBHANDLE): ${TOP_DIR}/src/dbhandle.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(MYSQLHANDLE): ${TOP_DIR}/src/mysqlhandle.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)

$(ORACLEHANDLE): ${TOP_DIR}/src/orahandle.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)	
	
$(DBAPI): ${TOP_DIR}/src/dbapi.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@ $(INCS)	
	
install:
	echo "inctalling ........................."
	
	
.PHONY:clean	
clean:
	rm -f $(TARGET) $(OBJS)
	
