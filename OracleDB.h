#ifndef _MY_ORACLEDB_H_INCLUDE_
#define _MY_ORACLEDB_H_INCLUDE_



int ConnectDB(const char* userName, const char* password, const char* sid);
int DisConnectDB();


#endif
