DBSQLAPI is a C++ library for accessing multiple SQL databases (Oracle, MySQL, Redis，Memcached...). It uses native APIs of target DBMS so applications developed with DBSQLAPI library run swiftly and efficiently.  By encapsulating a vendor's API, DBSQLAPI library acts as middleware and delivers database portability. 


Why use DBSQLAPI?

DBSQLAPI directly calls native API's of target DBMSs . That's why DBSQLAPI is the fastest way to manage you data.
No need (vs ADO) to install and configure OLEDB and/or ODBC drivers when developing and distributing your applications.
Low-level interface that allows developers to access database-specific features.
DBSQLAPI ships with all the sources.


Examples

Steps

Step 1. Connecting to database (and error handling)

Step 2. Executing a simple SQL command or cache command

Step 3. Binding input parameters if need

Step 4. Executing a select query (and fetching result set)

Step 5. Multithreading support and canceling queries
