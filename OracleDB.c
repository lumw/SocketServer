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
��������:���������ݿ������
����˵��:
    const char *userName    �û���
    const char *password    ����
    const char *sid         ���ݿ�ʵ����
���˵��:
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

    /*
    OCI_BindString(stmt, ":devID",        gps_info->device_id,    sizeof(gps_info->device_id));
    OCI_BindString(stmt, ":xcoordinate",  gps_info->x_coordinate, sizeof(gps_info->device_id));
    OCI_BindString(stmt, ":ycoordinate",  gps_info->Y_coordinate, sizeof(gps_info->device_id));
    OCI_BindString(stmt, ":high",         gps_info->height,       sizeof(gps_info->device_id));
    OCI_BindString(stmt, ":speed",        gps_info->speed,        sizeof(gps_info->device_id));
    OCI_BindString(stmt, ":orientation",  gps_info->direction,    sizeof(gps_info->device_id));
    */

    char device_id[34];
    char x_coordinate[8+1];
    char Y_coordinate[8+1];
    char height[8+1];
    char speed[8+1];
    char direction[8+1];


    memcpy(device_id,       gps_info+8, 33);
    memcpy(x_coordinate,    gps_info+41, 8);
    memcpy(Y_coordinate,    gps_info+49, 8);
    memcpy(height,          gps_info+57, 8);
    memcpy(speed,           gps_info+65, 8);
    memcpy(direction,       gps_info+73, 8);

    OCI_BindString(stmt, ":devID",        device_id,    sizeof(device_id));
    OCI_BindString(stmt, ":xcoordinate",  x_coordinate, sizeof(x_coordinate));
    OCI_BindString(stmt, ":ycoordinate",  Y_coordinate, sizeof(Y_coordinate));
    OCI_BindString(stmt, ":high",         height,       sizeof(height));
    OCI_BindString(stmt, ":speed",        speed,        sizeof(speed));
    OCI_BindString(stmt, ":orientation",  direction,    sizeof(direction));

    OCI_Execute(stmt);

    OCI_Commit(conn);
}



