#include "Transmit.h"
#include "log.h"
#include "socket_api.h"
#include "RecDeal.h"
#include "tools.h"
#include "Global.h"
#include <errno.h>
#include <string.h>

#define MAX_TRSMT_SERVICES  10      //转发业务的最大个数
#define HEAD_LENGTH         86      //H2消息包头的长度

extern int cur_port;
extern int cur_serial;
extern struct TRSMT_CONF *G_trsmt_conf;
extern struct INIT_CONF   G_ini;

int hw_query_fee_pre_deal(char *trans_buffer);
int hw_query_fee_post_deal(char *trans_buffer);
int zte_query_fee_pre_deal(char *trans_buffer);
int zte_query_fee_post_deal(char *trans_buffer);


/**************************************************************************************************
 *Function Name: init_trsmt_conf
 *
 *Parameters:
 *
 *Description: create and init a TRSMT_CONF
 *
 *Returns: the pointer to TRSMT_CONF which has been initialized with infomations of services needs
 *         to be transmitted.
 *
 **************************************************************************************************/
struct TRSMT_CONF *init_trsmt_conf()
{
    struct TRSMT_CONF *trsmt_conf_array = (struct TRSMT_CONF *)malloc(sizeof(struct TRSMT_CONF) * (MAX_TRSMT_SERVICES));

    if(trsmt_conf_array == NULL)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "为转发配置数据申请空间失败");

        return NULL;
    }

    //华为PPS用户余额查询
    memcpy(trsmt_conf_array[0].cus_service_code, "102013207001", 13);
    memcpy(trsmt_conf_array[0].trsmt_service_code, "102081030014", 13);
    trsmt_conf_array[0].pre_deal_func = hw_query_fee_pre_deal;
    trsmt_conf_array[0].post_deal_func = hw_query_fee_post_deal;
    trsmt_conf_array[0].target = 1;

    //中兴PPS用户余额查询
    memcpy(trsmt_conf_array[1].cus_service_code, "102013207002", 13);
    memcpy(trsmt_conf_array[1].trsmt_service_code, "102011040009", 13);
    trsmt_conf_array[1].pre_deal_func = zte_query_fee_pre_deal;
    trsmt_conf_array[1].post_deal_func = zte_query_fee_post_deal;
    trsmt_conf_array[1].target = 2;

    return trsmt_conf_array;
}


/**************************************************************************************************
 *Function Name: free_trsmt_conf
 *
 *Parameters: struct TRSMT_CONF *transmit_config
 *
 *Description: free the spaces which trsmt_conf_array points to.
 *
 *Returns: void
 *
 **************************************************************************************************/
void free_trsmt_conf(struct TRSMT_CONF *trsmt_conf_array)
{
    if(trsmt_conf_array != NULL)
    {
        free((void *)trsmt_conf_array);
    }

    trsmt_conf_array = NULL;
}


/**************************************************************************************************
 *Function Name: need_trsmt
 *
 *Parameters: const char *service_code
 *
 *Description: judge if the service specified by service_code needs transmit.
 *
 *Returns: -1:don't need transmit
 *        >=0:the serial number of a service in G_trsmt_conf which needs transmit.
 *         -2:error
 *
 **************************************************************************************************/
int need_trsmt(const char *service_code)
{
    int i;

    if(service_code == NULL)
    {
        return -2;
    }

    for(i = 0; i < MAX_TRSMT_SERVICES; i++)
    {
        if( strcmp(G_trsmt_conf[i].cus_service_code, service_code) == 0 )
        {
            return i;
        }
        else
        {
            continue;
        }
    }

    return -1;
}


/**************************************************************************************************
 *Function Name: transmit
 *
 *Parameters: char *trans_buffer    data to be transmited
 *            int conf_no           serial number of service transmit configration in
 *                                  G_trsmt_conf
 *            int is_rec            if record the transmit message 1:yes 0:no
 *Description: transmit the message specified by trans_buffer to the address specified by socket_fd.
 *
 *Returns: int -1:failed   0:success
 *
 **************************************************************************************************/



/**************************************************************************************************
 *Function Name: hw_query_fee_pre_deal
 *
 *Parameters: char *trans_buffer    data to be formatted before transmit to hw
 *
 *Description: format data specified by trans_buffer before transmit to hw
 *
 *Returns: int -1:failed   0:success
 *
 **************************************************************************************************/


/**************************************************************************************************
 *Function Name: hw_query_fee_post_deal
 *
 *Parameters: char *trans_buffer    data to be formatted after transmitted to hw
 *
 *Description: format data specified by trans_buffer after transmitted to hw
 *
 *Returns: int -1:failed   0:success
 *
 **************************************************************************************************/


/**************************************************************************************************
 *Function Name: zte_query_fee_pre_deal
 *
 *Parameters: char *trans_buffer    data to be formatted before transmit to zte
 *
 *Description: format data specified by trans_buffer before transmit to zte
 *
 *Returns: int -1:failed   0:success
 *
 **************************************************************************************************/


/**************************************************************************************************
 *Function Name: zte_query_fee_post_deal
 *
 *Parameters: char *trans_buffer    data to be formatted after transmitted to zte
 *
 *Description: format data specified by trans_buffer after transmitted to zte
 *
 *Returns: int -1:failed   0:success
 *
 **************************************************************************************************/

