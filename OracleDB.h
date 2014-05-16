#ifndef _MY_ORACLEDB_H_
#define _MY_ORACLEDB_H_

#define  MAX_PARAM   20
#define  THREAD_COUNT    10

int  DB_ConnectDB1(const char *user_name, const char *password, const char *db_id);
int  DB_DisconnectDB();
int  DB_DisconnectDB1();
int  DB_OCIBindSTR_A(int iStmthp, int iCount, const char *sql_var, char *sVar,int iVarLen);
int  DB_OCIBindSTR_B(int iStmthp, int iCount, char *sVar,int iVarLen);
int  DB_OCIBindINT_A(int iStmthp, int iCount, const char *sql_var, int *iVar);
int  DB_OCIBindINT_B(int iStmthp, int iCount, int *iVar);
int  DB_OCIPrepare(int iStmthp, char *ProcSql);
int  DB_OCIExec(int iStmthp, int type);
int  DB_OCIFetch(int iStmthp, int FetchNum);
void DB_report_error(OCIError *errhp);

int DB_SelectRecordBind(char *name,int *nPid, char *mycommand, int nLencmd,
    char *register_number, int nLenreg, char *card_sn, int cardSnLen, int *product_num, int *apply_event,char *error_code,int nLenError);
int  DB_SelectRecord();
int  DB_ModifyRecordStatus(char *name,char *register_number,int nLenreg, char *why, int nLenWhy, int *pick_status);
int  DB_CallBackRecord();
int  DB_ExecuteSQL(unsigned char *sql);
int  DB_ReadRollbackCommand();

#endif
