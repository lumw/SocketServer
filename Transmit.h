#ifndef _TUXEDO_REALINTF_TRANSMIT_H_

#define _TUXEDO_REALINTF_TRANSMIT_H_

struct TRSMT_CONF
{
	char cus_service_code[21];					/*客服业务编码*/
	char trsmt_service_code[21];				/*转发接口业务编码*/

	int (*pre_deal_func)(char *trans_buffer);	/*前处理函数*/
	int (*post_deal_func)(char *trans_buffer);	/*后处理函数*/

	int target;									/*转发目的地, 1:华为 2:中兴*/
};

struct TRSMT_CONF *init_trsmt_conf();
void free_trsmt_conf(struct TRSMT_CONF *trsmt_conf_array);
int need_trsmt(const char *service_code);
int transmit(char *trans_buffer, int sn_in_G_trsmt_conf, int is_rec);

#endif
