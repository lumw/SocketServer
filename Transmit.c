#include <stdio.h>
#include <stdlib.h>

#include "Transmit.h"

void Parse_Geographic_Information(const char *GeograInfo, struct Geogra_INFO *geogra_info)
{
    double x_coordinate;
    double Y_coordinate;
    double height;
    double speed;
    double direction;

    char s_x_coordinate[11 + 1];
    char s_Y_coordinate[11 + 1];
    char s_speed[8 + 1];
    char s_direction[6 + 1];
    char s_height[6 + 1];

    memset(s_x_coordinate, 0, sizeof(s_x_coordinate));
    memset(s_Y_coordinate, 0, sizeof(s_Y_coordinate));
    memset(s_speed, 0, sizeof(s_speed));
    memset(s_direction, 0, sizeof(s_direction));
    memset(s_height, 0, sizeof(s_height));

    memcpy(geogra_info->device_id,          GeograInfo + 11, 6);
    memcpy(geogra_info->warn_flag,          GeograInfo + 19, 1);
    memcpy(s_x_coordinate,                  GeograInfo + 20, 11);
    memcpy(s_Y_coordinate,                  GeograInfo + 31, 11);
    memcpy(s_speed,                         GeograInfo + 42, 8);
    memcpy(s_direction,                     GeograInfo + 50, 6);
    memcpy(s_height,                        GeograInfo + 56, 6);
    memcpy(geogra_info->base_station_info1, GeograInfo + 74, 5);
    memcpy(geogra_info->base_station_info2, GeograInfo + 79, 5);
    memcpy(geogra_info->sent_msg_cnt,       GeograInfo + 84, 5);

    sscanf(s_x_coordinate,  "%lf",   &(geogra_info->x_coordinate));
    sscanf(s_Y_coordinate,  "%lf",   &(geogra_info->y_coordinate));
    sscanf(s_height,        "%lf",   &(geogra_info->height));
    sscanf(s_speed,         "%lf",   &(geogra_info->speed));
    sscanf(s_direction,     "%lf",   &(geogra_info->direction));
}



int Deal_Geographic_Information(const char *GeograInfo)
{
    struct Geogra_INFO *geogra_info;

    Parse_Geographic_Information(GeograInfo, geogra_info);

    if ( GPSInfoUpdate(GeograInfo) == ERROR )
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "更新实时表出错");
    }

    if ( GPSInfoCommit(GeograInfo) == ERROR )
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "写入历史表出错");
    }
}



