/********************************************************************************************************:
Author   : the.lumw@gmail.com
文件描述 :
    OCILib接口封装函数(实现文件部分)

修改记录 :
========================================================================================================
序号    修改日期            修改人                      修改原因
1       2014-05-22          the.lumw@gmail.com          创建版本
********************************************************************************************************/


#include "ocilib.h"
#include "OracleDB.h"
#include "Global.h"


OCI_Connection *conn = NULL;
OCI_Statement  *stmt = NULL;
OCI_Error      *err  = NULL;
OCI_Resultset  *rs   = NULL;



/****************************************************************************************************
函数功能:数据库错误记录函数
参数说明:
    const char *userName    用户名
    const char *password    口令
    const char *sid         数据库实例名
输出说明:
****************************************************************************************************/
void err_handler(OCI_Error *err)
{
    printf(
               "code  : ORA-%05i\n"
               "msg   : %s\n"
               "sql   : %s\n",
               OCI_ErrorGetOCICode(err),
               OCI_ErrorGetString(err),
               OCI_GetSql(OCI_ErrorGetStatement(err))
           );
}


/****************************************************************************************************
函数功能:建立与数据库的连接
参数说明:
    const char *userName    用户名
    const char *password    口令
    const char *sid         数据库实例名
输出说明:
****************************************************************************************************/
int ConnectDB(const char* userName, const char* password, const char* sid)
{

    if( !OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT) )
    {
        return ERROR;
    }

    conn = OCI_ConnectionCreate(sid, userName, password, OCI_SESSION_DEFAULT);

    return conn == NULL ? ERROR : RIGHT;
}


/****************************************************************************************************
函数功能:断开与数据库的连接
参数说明:

输出说明:

****************************************************************************************************/
int DisConnectDB()
{
    OCI_Cleanup();
    return RIGHT;
}


/****************************************************************************************************
函数功能:验证终端设备是否合法
参数说明:

输出说明:
        RIGHT 合法
        ERROR 不合法
****************************************************************************************************/
int check_terminal_legality(const char* terminalID)
{
    stmt = OCI_StatementCreate(conn);

    OCI_Prepare(stmt, "SELECT TO_CHAR(SYSDATE) FROM DUAL");

    OCI_Execute(stmt);

    rs = OCI_GetResultset(stmt);

    while( OCI_FetchNext(rs) )
    {
        printf("%s\n", OCI_GetString(rs, 1));
    }
}

