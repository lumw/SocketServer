/********************************************************************************************************:
Author   : the.lumw@gmail.com
�ļ����� :
    OCILib�ӿڷ�װ����(ʵ���ļ�����)

�޸ļ�¼ :
========================================================================================================
���    �޸�����            �޸���                      �޸�ԭ��
1       2014-05-22          the.lumw@gmail.com          �����汾
********************************************************************************************************/


#include "ocilib.h"
#include "OracleDB.h"
#include "Global.h"


OCI_Connection *conn = NULL;
OCI_Statement  *stmt = NULL;
OCI_Error      *err  = NULL;
OCI_Resultset  *rs   = NULL;

char *oci_error_msg;

int CommitCnt = 0;



/****************************************************************************************************
��������:���ݿ�����¼����
����˵��:
    const char *userName    �û���
    const char *password    ����
    const char *sid         ���ݿ�ʵ����
���˵��:
****************************************************************************************************/
void err_handler(OCI_Error *err)
{
    sprintf(
       oci_error_msg,
       "code  : ORA-%05i\n"
       "msg   : %s\n"
       "sql   : %s\n",
       OCI_ErrorGetOCICode(err),
       OCI_ErrorGetString(err),
       OCI_GetSql(OCI_ErrorGetStatement(err))
       );
}

/****************************************************************************************************
��������:���������ݿ������
����˵��:
    const char *userName    �û���
    const char *password    ����
    const char *sid         ���ݿ�ʵ����
���˵��:
****************************************************************************************************/
int ConnectDB(const char *userName, const char *password, const char *sid)
{

    if( !OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT) )

    {
        return ERROR;
    }

    conn = OCI_ConnectionCreate(sid, userName, password, OCI_SESSION_DEFAULT);

    return conn == NULL ? ERROR : RIGHT;
}


/****************************************************************************************************
��������:�Ͽ������ݿ������
����˵��:

���˵��:

****************************************************************************************************/
int DisConnectDB()
{
    OCI_Cleanup();
    return RIGHT;
}


/****************************************************************************************************
��������:��֤�ն��豸�Ƿ�Ϸ�
����˵��:

���˵��:
        RIGHT �Ϸ�
        ERROR ���Ϸ�
****************************************************************************************************/
int check_terminal_legality(const char *terminalID)
{
    stmt = OCI_StatementCreate(conn);

    OCI_Prepare(stmt, "SELECT TO_CHAR(SYSDATE) FROM DUAL");

    OCI_Execute(stmt);

    rs = OCI_GetResultset(stmt);

    while (OCI_FetchNext(rs))
    {
        printf("%s\n", OCI_GetString(rs, 1));
    }
}


/****************************************************************************************************
��������:���ն��豸�ϴ�����Ϣд�����ݿ���
����˵��:

���˵��:
        RIGHT �Ϸ�
        ERROR ���Ϸ�
****************************************************************************************************/
int gps_info_insert(const char *gps_info)
{


    stmt = OCI_StatementCreate(conn);
    OCI_Prepare(stmt, "insert into t_real_loc(DEVID, XCOORDINATE, YCOORDINATE, HIGH, SPEED, ORIENTATION, CREATEDATE) "
                "               values(:devID, :xcoordinate, :ycoordinate, :high, :speed, :orientation, sysdate) ");


    char device_id[34];
    double x_coordinate;
    double Y_coordinate;
    float height;
    float speed;
    float direction;


    memcpy(device_id,       gps_info + 6,  2);
    sscanf(gps_info, "%lf", &x_coordinate);
    sscanf(gps_info, "%lf", &Y_coordinate);
    sscanf(gps_info, "%f",  &height);
    sscanf(gps_info, "%f",  &speed);
    sscanf(gps_info, "%f",  &direction);


    OCI_BindString(stmt, ":devID",        device_id,    sizeof(device_id));
    OCI_BindDouble(stmt, ":xcoordinate",  &x_coordinate);
    OCI_BindDouble(stmt, ":ycoordinate",  &Y_coordinate);
    OCI_BindFloat(stmt,  ":high",         &height);
    OCI_BindFloat(stmt,  ":speed",        &speed);
    OCI_BindFloat(stmt,  ":orientation",  &direction);

    OCI_Execute(stmt);

    OCI_Commit(conn);

    OCI_StatementFree(stmt);
}
