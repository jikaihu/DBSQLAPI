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

OBJS=${DBSQLOBJ} $(ORACLEAPIOBJ) $(MYSQLAPIOBJ) $(VAROBJ) $(NVLISTOBJ) ${DBPUBOBJ} ${DBHANDLE} ${MYSQLHANDLE} ${ORACLEHANDLE}

TARGET=${TOP_DIR}/libdbsql.so


LIBS = 

INCS = -I./  -I${TOP_DIR}/rdb-inc/oracle -I${TOP_DIR}/rdb-inc/mysql -I${TOP_DIR}/rdb-inc/redis -I${TOP_DIR}/rdb-inc/memcached


CXXFLAGS = -g -fpic -DSYS_VERSION=\"$(VERSION)\"
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

install:
	echo "inctalling ........................."
	
	
.PHONY:clean	
clean:
	rm -f $(TARGET) $(OBJS)
	
