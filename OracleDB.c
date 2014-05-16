#include "Global.h"
#include "OracleDB.h"
#include <errno.h>
#include <oci.h>

int  DB_logged_on;
char DB_szErrorInfo[2048];
char buffer[1024];

OCIEnv     *envhp;
OCIError   *errhp;
OCIServer  *srvhp;
OCISvcCtx  *svchp;
OCISession *authp;
OCIStmt    *stmthp[THREAD_COUNT];
OCIBind    *bndhp[MAX_PARAM];
OCIDefine  *dfnhp[MAX_PARAM];

/****************************************************************************************************
函数功能:建立与数据库的连接
参数说明:
    const char *user_name   用户名
    const char *password    口令
    const char *db_id   数据库实例名
输出说明:
    RIGHT   表示建立连接成功
    ERROR   表示建立连接失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int  DB_ConnectDB1(const char *user_name, const char *password, const char *db_id)
{
    int i;
    DB_logged_on = ERROR;

    strcpy(DB_szErrorInfo, "");
    /* Initialize the OCI Process */
    if (OCIInitialize((ub4) OCI_DEFAULT, (dvoid *)0, (dvoid * (*)(dvoid *, size_t)) 0, (dvoid * (*)(dvoid *, dvoid *, size_t))0, (void (*)(dvoid *, dvoid *)) 0 ))
    {
        strcat(DB_szErrorInfo, "error: OCIInitialize(); \n");
        return ERROR;
    }
    /* Inititialize the OCI Environment */
    if (OCIEnvInit((OCIEnv **)&envhp, (ub4) OCI_DEFAULT, (size_t) 0, (dvoid **) 0 ))
    {
        strcat(DB_szErrorInfo, "error: OCIEnvInit(); \n");
        return ERROR;
    }

    /* Allocate a service handle */
    if (OCIHandleAlloc((dvoid *)envhp, (dvoid **)&svchp, (ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
    {
        strcat(DB_szErrorInfo, "error: OCIHandleAlloc() on svchp; \n");
        return ERROR;
    }

    /* Allocate an error handle */
    if (OCIHandleAlloc((dvoid *)envhp, (dvoid **)&errhp, (ub4) OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0))
    {
        strcat(DB_szErrorInfo, "error: OCIHandleAlloc() on errhp; \n");
        return ERROR;
    }

    /* Allocate a server handle */
    if (OCIHandleAlloc((dvoid *)envhp, (dvoid **)&srvhp, (ub4) OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0))
    {
        strcat(DB_szErrorInfo, "error: OCIHandleAlloc() on srvhp; \n");
        return ERROR;
    }

    /* Allocate a authentication handle */
    if (OCIHandleAlloc((dvoid *)envhp, (dvoid **)&authp, (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0))
    {
        strcat(DB_szErrorInfo, "error: OCIHandleAlloc() on authp; \n");
        return ERROR;
    }
    /* Allocate a statment handle */
    for (i = 0 ; i < THREAD_COUNT ; i++)
    {
        if (OCIHandleAlloc((dvoid *) envhp, (dvoid **) &stmthp[i], (ub4) OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0))
        {
            sprintf(buffer, "error: OCIHandleAlloc() on stmthp[%d]; \n", i);
            strcat(DB_szErrorInfo, buffer);
            return ERROR;
        }
    }

    //连接数据库
    if (strcmp(db_id, "local") == 0)   /*本地数据库使用默认的oracle_sid*/
    {
        if (OCIServerAttach(srvhp, errhp, (text *) 0, (sb4) 0, (ub4) OCI_DEFAULT))
        {
            strcat(DB_szErrorInfo, "error: OCIServerAttach(); \n");
            DB_report_error(errhp);
            return ERROR;
        }
    }
    else  /*远程数据库使用配置的oracle_sid*/
    {
        if (OCIServerAttach(srvhp, errhp, (text *) db_id, (sb4) strlen((char *)db_id), (ub4) OCI_DEFAULT))
        {
            strcat(DB_szErrorInfo, "error: OCIServerAttach(); \n");
            DB_report_error(errhp);
            return ERROR;
        }
    }

    /* Set the server handle in the service handle */
    if (OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) srvhp, (ub4) 0, (ub4) OCI_ATTR_SERVER, errhp))
    {
        strcat(DB_szErrorInfo, "error: OCIAttrSet() server attribute; \n");
        DB_report_error(errhp);
        return ERROR;
    }
    /* Set attributes in the authentication handle */
    if (OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION, (dvoid *) user_name, (ub4) strlen((char *) user_name), (ub4) OCI_ATTR_USERNAME, errhp))
    {
        strcat(DB_szErrorInfo, "error: OCIAttrSet() userid;");
        DB_report_error(errhp);
        return ERROR;
    }
    if (OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION, (dvoid *) password, (ub4) strlen((char *) password), (ub4) OCI_ATTR_PASSWORD, errhp))
    {
        strcat(DB_szErrorInfo, "error: OCIAttrSet() passwd; \n");
        DB_report_error(errhp);
        return ERROR;
    }

    if (OCISessionBegin(svchp, errhp, authp,(ub4) OCI_CRED_RDBMS,(ub4) OCI_DEFAULT))
    {
        strcat(DB_szErrorInfo, "error: OCISessionBegin(); \n");
        DB_report_error(errhp);
        return ERROR;
    }
    /* Set the authentication handle in the Service handle */
    if (OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX, (dvoid *) authp, (ub4) 0, (ub4) OCI_ATTR_SESSION, errhp))
    {
        strcat(DB_szErrorInfo, "error: OCIAttrSet() session; \n");
        DB_report_error(errhp);
        return ERROR;
    }

    DB_logged_on = RIGHT;
    return RIGHT;

}



/****************************************************************************************************
函数功能:断开与数据库的连接
参数说明:
    无
输出说明:
    RIGHT   表示建立连接成功
    ERROR   表示建立连接失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_DisconnectDB1()
{
    int  i;

    strcpy(DB_szErrorInfo, "");
    if (DB_logged_on == RIGHT)
    {
        if (OCISessionEnd(svchp, errhp, authp, (ub4) 0))
        {
            strcpy(DB_szErrorInfo, "error: OCISessionEnd(); \n");
            DB_report_error(errhp);

        }
        else
            strcat(DB_szErrorInfo, "退出数据库成功; \n");

        if (OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT))
        {
            strcat(DB_szErrorInfo, "error: OCIServerDetach(); \n");
            DB_report_error(errhp);

        }
        else
            strcat(DB_szErrorInfo, "断开数据库连接成功. \n");
    }

    for (i = 0 ; i < THREAD_COUNT ; i++)
    {
        if (stmthp[i])
        {
            if (OCIHandleFree((dvoid *) stmthp[i], (ub4) OCI_HTYPE_STMT))
            {
                sprintf(buffer, "Freeing handles stmthp[%d] faild! \n",i);
                strcat(DB_szErrorInfo, buffer);
            }
        }
    }

    if (srvhp)
    {
        if (OCIHandleFree((dvoid *) srvhp, (ub4) OCI_HTYPE_SERVER))
        {
            strcat(DB_szErrorInfo, "error:freeing handles srvhp failed! \n");
        }
    }

    if (svchp)
    {
        if (OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX))
        {
            strcat(DB_szErrorInfo, "error:freeing handles svchp failed! \n");
        }
    }

    if (errhp)
    {
        if (OCIHandleFree((dvoid *) errhp, (ub4) OCI_HTYPE_ERROR))
        {
            strcat(DB_szErrorInfo, "error:freeing handles errhp failed! \n");
        }
    }

    if (authp)
    {
        if (OCIHandleFree((dvoid *) authp, (ub4) OCI_HTYPE_SESSION))
        {
            strcat(DB_szErrorInfo, "Freeing handles authp failed! \n");
        }
    }

    if (envhp)
    {
        if (OCIHandleFree((dvoid *) envhp, (ub4) OCI_HTYPE_ENV))
        {
            strcat(DB_szErrorInfo, "Freeing handles envhp failed! \n");
        }
    }

    strcat(DB_szErrorInfo, "Free handles end\n");

    return RIGHT;
}

/****************************************************************************************************
函数功能:打印数据库错误信息
参数说明:
    无
输出说明:
    无
****************************************************************************************************/
void DB_report_error(OCIError *errhp)
{
    text msgbuf[512];
    sb4  errcode = 0;
    //char buffer[128];

    (void) OCIErrorGet((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode, msgbuf, (ub4) sizeof(msgbuf), (ub4) OCI_HTYPE_ERROR);
    if (errcode != 0)
    {
        sprintf(buffer, "ERROR_CODE :%d ", errcode);
        strcat(DB_szErrorInfo, buffer);
        strcat(DB_szErrorInfo, (char *)msgbuf);
    }
    return;
}


/****************************************************************************************************
函数功能:通过标识法绑定一个字符串变量
参数说明:
    int iStmthp 被绑定的游标的序号
    int iCount  该变量是sql语句的第几个变量
    char *sql_var   变量在sql语句中的表示形式
    char *sVar  绑定的变量
    int iVarLen 绑定变量的长度
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIBindSTR_A(int iStmthp, int iCount, const char *sql_var, char *sVar,int iVarLen)
{
    //char buffer[1024];
    bndhp[iCount] = (OCIBind *)0;

    if (OCIBindByName(stmthp[iStmthp], &bndhp[iCount], errhp, (text *) sql_var, (sb4) strlen(sql_var),(dvoid *)sVar, (sb4) iVarLen, SQLT_STR, (dvoid *) 0, (ub2 *)0, (ub2 *)0,(ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT))
    {
        sprintf(buffer, "OCIBindByName Error: Parameter %s bind error! \n", sql_var);
        strcat(DB_szErrorInfo, buffer);
        DB_report_error(errhp);
        return ERROR;
    }

    return RIGHT;

}

/****************************************************************************************************
函数功能:通过位置法绑定一个字符串变量
参数说明:
    int iStmthp 被绑定的游标的序号
    int iCount  该变量是sql语句的第几个变量
    char *sVar  绑定的变量
    int iVarLen 绑定变量的长度
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIBindSTR_B(int iStmthp, int iCount, char *sVar,int iVarLen)
{
    //char buffer[128];
    int iTmp = iCount + 1;
    dfnhp[iCount] = (OCIDefine *) 0;

    if (OCIDefineByPos(stmthp[iStmthp], &dfnhp[iCount], errhp, (ub4) iCount + 1, (dvoid *) sVar, (sb4) iVarLen, (ub2) SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) OCI_DEFAULT))
    {
        sprintf(buffer, "OCIDefineByPos Error: Parameter %d bind error! \n", iCount);
        strcat(DB_szErrorInfo, buffer);
        DB_report_error(errhp);
        return ERROR;
    }
    return RIGHT;
}

/****************************************************************************************************
函数功能:通过标识法绑定一个整型变量
参数说明:
    int iStmthp 被绑定的游标的序号
    int iCount  该变量是sql语句的第几个变量
    char *sql_var   变量在sql语句中的表示形式
    int &iVar   绑定的变量
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIBindINT_A(int iStmthp,int iCount, const char *sql_var, int *iVar)
{
    //char buffer[1024];
    bndhp[iCount]= (OCIBind *) 0;
    if (OCIBindByName(stmthp[iStmthp],&bndhp[iCount], errhp, (text *) sql_var, -1, (dvoid *) iVar, (sword) sizeof(int), SQLT_INT, (dvoid *) 0, (ub2 *)0, (ub2 *)0, (ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT))
    {
        sprintf(buffer, "OCIBindByName Error: Parameter %s bind error! \n", sql_var);
        strcat(DB_szErrorInfo, buffer);
        DB_report_error(errhp);
        return ERROR;
    }
    return RIGHT;

}

/****************************************************************************************************
函数功能:通过位置法绑定一个整型变量
参数说明:
    int iStmthp 被绑定的游标的序号
    int iCount  该变量是sql语句的第几个变量
    int &iVar   绑定的变量
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIBindINT_B(int iStmthp, int iCount, int *iVar)
{
    //char buffer[128];
    dfnhp[iCount] = (OCIDefine *) 0;

    if (OCIDefineByPos(stmthp[iStmthp], &dfnhp[iCount], errhp, (ub4) iCount+1, (dvoid *) iVar, (sb4) sizeof(int), (ub2) SQLT_INT, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) OCI_DEFAULT))
    {
        sprintf(buffer, "OCIDefineByPos Error: Parameter %d bind error! \n", iCount);
        strcat(DB_szErrorInfo, buffer);
        DB_report_error(errhp);
        return ERROR;
    }
    return RIGHT;
}


/****************************************************************************************************
函数功能:执行sql语句之前的分析
参数说明:
    int iStmthp 被绑定的游标的序号
    char *ProcSql   分析的sql语句
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIPrepare(int iStmthp,char *ProcSql)
{
    if (OCIStmtPrepare(stmthp[iStmthp], errhp, (text *)ProcSql, (ub4)strlen(ProcSql), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
    {
        strcat(DB_szErrorInfo, "error:OCIStmtPrepare Error!\n ");
        strcat(DB_szErrorInfo, ProcSql);
        DB_report_error(errhp);
        return ERROR;
    }
    return RIGHT;
}

/****************************************************************************************************
函数功能:执行sql语句
参数说明:
    int iStmthp 被绑定的游标的序号
    int type
        =0 执行一个只有一行返回或者没有返回的sql语句
        =1 执行一个有多行返回的sql语句
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIExec(int iStmthp,int type)
{
    int iRetcode;

    if (type == 0)
    {
        if (OCIStmtExecute(svchp, stmthp[iStmthp], errhp, (ub4) 1, (ub4) 0,  0, 0, (ub4) OCI_DEFAULT))
        {
            strcat(DB_szErrorInfo, "OCIStmtExecute Error!\n");
            DB_report_error(errhp);
            sprintf(buffer, "error: OCIInitialize();%d:%s", errno, strerror(errno));
            printf("err=[%s]\n", buffer);

            return ERROR;
        }
    }
    else if (type == 1)
    {
        iRetcode = OCIStmtExecute(svchp, stmthp[iStmthp], errhp, (ub4) 0, (ub4) 0, (CONST OCISnapshot*) 0, (OCISnapshot*) 0,(ub4) OCI_DEFAULT);
        if (iRetcode)
        {
            strcat(DB_szErrorInfo, "OCIStmtExecute Error!\n");
            DB_report_error(errhp);
            if (iRetcode == OCI_NO_DATA)
                strcat(DB_szErrorInfo, "OCI_NO_DATA happend!\n");
            return ERROR;
        }
    }

    return RIGHT;
}

/****************************************************************************************************
函数功能:fetch一个有多行返回或者没有返回的sql语句
参数说明:
    int iStmthp 被绑定的游标的序号
    int FetchNum    一次提取的记录的行数
输出说明:
    RIGHT   表示执行成功
    ERROR   表示执行失败 则可以通过GetErrorInfo()函数取得错误信息
****************************************************************************************************/
int DB_OCIFetch(int iStmthp,int FetchNum)
{
    int iRetcode;

    //开始Fetch
    iRetcode = OCIStmtFetch(stmthp[iStmthp],errhp,FetchNum,OCI_FETCH_NEXT,OCI_DEFAULT);
    if (iRetcode)
    {
        if (iRetcode == OCI_NO_DATA)
            return ERROR;
        strcat(DB_szErrorInfo, "error:OCIStmtFetch Error!\n");
        DB_report_error(errhp);
        return ERROR;
    }
    return RIGHT;
}


int DB_DisconnectDB()
{
    return DB_DisconnectDB1();
}

int DB_SelectRecordBind(char *name,int *nPid, char *mycommand, int nLencmd,
                        char *register_number, int nLenreg, char *card_sn, int cardSnLen, int *product_num, int *apply_event,char *error_code,int nLenError)
{
    char sql[1024];//, buffer[1024];

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "begin %s(%d, :os_command, :os_register_number, :os_card_sn, :on_prod_num, :on_apply_event, :os_error_code); end;", name,nPid);

    if (DB_OCIPrepare(1, sql) == ERROR)
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo,"分析 %s 语句失败! \n%s", name, buffer);
        printf("DB_szErrorInfo = [%s]\n", DB_szErrorInfo);
        return ERROR;
    }

    if (//(DB_OCIBindINT_A(1, 0, ":in_pid", nPid) == ERROR) ||
        (DB_OCIBindSTR_A(1, 0, ":os_command", mycommand,nLencmd) == ERROR) ||
        (DB_OCIBindSTR_A(1, 1, ":os_register_number", register_number,(sword)nLenreg) == ERROR) ||
        (DB_OCIBindSTR_A(1, 2, ":os_card_sn", card_sn, cardSnLen) == ERROR) ||
        (DB_OCIBindINT_A(1, 3, ":on_prod_num", product_num) == ERROR) ||
        (DB_OCIBindINT_A(1, 4, ":on_apply_event", apply_event) == ERROR) ||
        (DB_OCIBindSTR_A(1, 5, ":os_error_code", error_code, nLenError) == ERROR))
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo, "%s 变量绑定失败! \n%s", name, buffer);
        printf("DB_szErrorInfo = [%s]\n", DB_szErrorInfo);
        return ERROR;
    }

    return RIGHT;
}

int DB_SelectRecord()
{
    if (DB_OCIExec(1, 0) == ERROR)
    {
        return ERROR;
    }

    return RIGHT;
}

int  DB_ModifyRecordStatus(char *name,char *register_number,int nLenreg, char *why, int nLenWhy, int *pick_status)
{
    char sql[1024];//, buffer[1024];
    memset(sql, 0, sizeof(sql));

    sprintf(sql, "begin %s(:v_register_number, :v_fail_reason,:v_flag); end;", name);

    if (DB_OCIPrepare(2, sql) == ERROR)
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo,"分析 %s 语句失败! \n%s", name, buffer);
        return ERROR;
    }

    if ((DB_OCIBindSTR_A(2, 0, ":v_register_number", register_number,nLenreg) == ERROR) ||
        //(DB_OCIBindSTR_A(2, 1, ":v_bus_string1_ret", bus_str1_ret,nLen1) == ERROR) ||
        //(DB_OCIBindSTR_A(2, 2, ":v_bus_string2_ret", bus_str2_ret,nLen2) == ERROR) ||
        (DB_OCIBindSTR_A(2, 1, ":v_fail_reason",     why,nLenWhy) == ERROR) ||
        (DB_OCIBindINT_A(2, 2, ":v_flag",            pick_status) == ERROR))
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo,"%s 变量绑定失败! \n%s", name, buffer);
    }

    return RIGHT;
}

int  DB_CallBackRecord()
{
    if (DB_OCIExec(2, 0) == ERROR)
    {
        return ERROR;
    }

    return RIGHT;
}

int DB_ExecuteSQL(unsigned char *sql)
{
/*    char buffer[1024];*/
    //char buffer[2048];

    if (DB_OCIPrepare(3, (char*)sql) == ERROR)
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo,"分析 %s 语句失败! \n%s", sql, buffer);
        return ERROR;
    }

    if (DB_OCIExec(3, 0) == ERROR)
    {
        strcpy(buffer, DB_szErrorInfo);
        sprintf(DB_szErrorInfo, "执行 %s 语句失败! \n%s", sql, buffer);
        return ERROR;
    }

    return RIGHT;
}
