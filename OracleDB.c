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

char oci_error_msg[2048];

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
    memset(oci_error_msg, 0, sizeof(oci_error_msg));

    sprintf(oci_error_msg,
            "ErrCode  : ORA-%05i  ErrString:%s\n"
            "SQL: %s\n",
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

    if ( !OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT) )

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
    if ( !OCI_Cleanup() )
    {
        return ERROR;
    }

    return RIGHT;
}

/****************************************************************************************************
��������:��ȡ���������ݿ����Ϣ
����˵��:

���˵��:

****************************************************************************************************/
int getDBInfo(char *DBInfo)
{
    sprintf(DBInfo, "Server major    version : %i\n"
                    "Server minor    version : %i\n"
                    "Server revision version : %i\n"
                    "Connection      version : %i\n",

                OCI_GetServerMajorVersion(conn),
                OCI_GetServerMinorVersion(conn),
                OCI_GetServerRevisionVersion(conn),
                OCI_GetVersionConnection(conn)
            );
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
��������:���ն��豸�ϴ�����Ϣд����ʷ����
����˵��:

���˵��:
        RIGHT �ɹ�
        ERROR ʧ��
****************************************************************************************************/
int GPSInfoCommit(const char *gps_info)
{


    stmt = OCI_StatementCreate(conn);
    if ( !OCI_Prepare(stmt, "insert into t_history_loc(DEVID, XCOORDINATE, YCOORDINATE, HIGH, SPEED, ORIENTATION, CREATEDATE) "
                       "      values(:devID, :xcoordinate, :ycoordinate, :high, :speed, :orientation, sysdate) ") )
    {

        return ERROR;
    }


    char device_id[6 + 1];
    double x_coordinate;
    double Y_coordinate;
    double height;
    double speed;
    double direction;

    char warn_flag[1 + 1];
    char s_x_coordinate[11 + 1];
    char s_Y_coordinate[11 + 1];
    char s_speed[8 + 1];
    char s_direction[6 + 1];
    char s_height[6 + 1];
    char base_station_info1[2 + 1];
    char base_station_info2[2 + 1];
    char sent_msg_cnt[4 + 1];

    memset(device_id, 0, sizeof(device_id));
    memset(warn_flag, 0, sizeof(warn_flag));
    memset(s_x_coordinate, 0, sizeof(s_x_coordinate));
    memset(s_Y_coordinate, 0, sizeof(s_Y_coordinate));
    memset(s_speed, 0, sizeof(s_speed));
    memset(s_direction, 0, sizeof(s_direction));
    memset(s_height, 0, sizeof(s_height));
    memset(base_station_info1, 0, sizeof(base_station_info1));
    memset(base_station_info2, 0, sizeof(base_station_info2));
    memset(sent_msg_cnt, 0, sizeof(sent_msg_cnt));




    memcpy(device_id,           gps_info + 11, 6);
    memcpy(warn_flag,           gps_info + 19, 1);
    memcpy(s_x_coordinate,      gps_info + 20, 11);
    memcpy(s_Y_coordinate,      gps_info + 31, 11);
    memcpy(s_speed,             gps_info + 42, 8);
    memcpy(s_direction,         gps_info + 50, 6);
    memcpy(s_height,            gps_info + 56, 6);
    memcpy(base_station_info1,  gps_info + 74, 5);
    memcpy(base_station_info2,  gps_info + 79, 5);
    memcpy(sent_msg_cnt,        gps_info + 84, 5);

    trim(device_id);
    trim(s_x_coordinate);
    trim(s_Y_coordinate);
    trim(s_height);
    trim(s_speed);
    trim(s_direction);

    //printf("[%s] [%s] [%s] [%s] [%s]\n", s_x_coordinate, s_Y_coordinate, s_speed, s_direction, s_height);

    sscanf(s_x_coordinate,  "%lf",   &x_coordinate);
    sscanf(s_Y_coordinate,  "%lf",   &Y_coordinate);
    sscanf(s_height,        "%lf",   &height);
    sscanf(s_speed,         "%lf",   &speed);
    sscanf(s_direction,     "%lf",   &direction);

    //printf("����ת���������\n");

    OCI_BindString(stmt, ":devID",          device_id,    sizeof(device_id));
    OCI_BindDouble(stmt, ":xcoordinate",    &x_coordinate);
    OCI_BindDouble(stmt, ":ycoordinate",    &Y_coordinate);
    OCI_BindDouble(stmt,  ":high",          &height);
    OCI_BindDouble(stmt,  ":speed",         &speed);
    OCI_BindDouble(stmt,  ":orientation",   &direction);


    if ( !OCI_Execute(stmt) )
    {
        return ERROR;
    }

    if ( !OCI_Commit(conn) )
    {
        return ERROR;
    }

    if ( !OCI_StatementFree(stmt) )
    {
        return ERROR;
    }

    return RIGHT;

}


int GPSInfoUpdate(const char *gps_info)
{

    stmt = OCI_StatementCreate(conn);
    OCI_Prepare(stmt, "update  t_real_loc set XCOORDINATE = :xcoordinate, YCOORDINATE = :ycoordinate, HIGH = :high,"
                "SPEED = :speed, ORIENTATION = :orientation, CREATEDATE = sysdate where DEVID = :devID");


    char device_id[6 + 1];
    double x_coordinate;
    double Y_coordinate;
    double height;
    double speed;
    double direction;

    char warn_flag[1 + 1];
    char s_x_coordinate[11 + 1];
    char s_Y_coordinate[11 + 1];
    char s_speed[8 + 1];
    char s_direction[6 + 1];
    char s_height[6 + 1];
    char base_station_info1[2 + 1];
    char base_station_info2[2 + 1];
    char sent_msg_cnt[4 + 1];

    memset(device_id, 0, sizeof(device_id));
    memset(warn_flag, 0, sizeof(warn_flag));
    memset(s_x_coordinate, 0, sizeof(s_x_coordinate));
    memset(s_Y_coordinate, 0, sizeof(s_Y_coordinate));
    memset(s_speed, 0, sizeof(s_speed));
    memset(s_direction, 0, sizeof(s_direction));
    memset(s_height, 0, sizeof(s_height));
    memset(base_station_info1, 0, sizeof(base_station_info1));
    memset(base_station_info2, 0, sizeof(base_station_info2));
    memset(sent_msg_cnt, 0, sizeof(sent_msg_cnt));




    memcpy(device_id,           gps_info + 11, 6);
    memcpy(warn_flag,           gps_info + 19, 1);
    memcpy(s_x_coordinate,      gps_info + 20, 11);
    memcpy(s_Y_coordinate,      gps_info + 31, 11);
    memcpy(s_speed,             gps_info + 42, 8);
    memcpy(s_direction,         gps_info + 50, 6);
    memcpy(s_height,            gps_info + 56, 6);
    memcpy(base_station_info1,  gps_info + 74, 5);
    memcpy(base_station_info2,  gps_info + 79, 5);
    memcpy(sent_msg_cnt,        gps_info + 84, 5);

    trim(device_id);
    trim(s_x_coordinate);
    trim(s_Y_coordinate);
    trim(s_height);
    trim(s_speed);
    trim(s_direction);

    //printf("[%s] [%s] [%s] [%s] [%s]\n", s_x_coordinate, s_Y_coordinate, s_speed, s_direction, s_height);

    sscanf(s_x_coordinate, "%lf",   &x_coordinate);
    sscanf(s_Y_coordinate, "%lf",   &Y_coordinate);
    sscanf(s_height,        "%lf",   &height);
    sscanf(s_speed,         "%lf",   &speed);
    sscanf(s_direction,     "%lf",   &direction);

    //printf("����ת���������\n");

    OCI_BindString(stmt, ":devID",        device_id,    sizeof(device_id));
    OCI_BindDouble(stmt, ":xcoordinate",  &x_coordinate);
    OCI_BindDouble(stmt, ":ycoordinate",  &Y_coordinate);
    OCI_BindDouble(stmt,  ":high",         &height);
    OCI_BindDouble(stmt,  ":speed",        &speed);
    OCI_BindDouble(stmt,  ":orientation",  &direction);

    if ( !OCI_Execute(stmt) )
    {
        return ERROR;
    }

    if ( !OCI_Commit(conn) )
    {
        return ERROR;
    }

    if ( !OCI_StatementFree(stmt) )
    {
        return ERROR;
    }

    return RIGHT;
}
