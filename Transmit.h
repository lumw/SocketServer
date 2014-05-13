#ifndef _TUXEDO_REALINTF_TRANSMIT_H_

#define _TUXEDO_REALINTF_TRANSMIT_H_

struct TRSMT_CONF
{
	char cus_service_code[21];					/*�ͷ�ҵ�����*/
	char trsmt_service_code[21];				/*ת���ӿ�ҵ�����*/

	int (*pre_deal_func)(char *trans_buffer);	/*ǰ������*/
	int (*post_deal_func)(char *trans_buffer);	/*������*/

	int target;									/*ת��Ŀ�ĵ�, 1:��Ϊ 2:����*/
};

struct TRSMT_CONF *init_trsmt_conf();
void free_trsmt_conf(struct TRSMT_CONF *trsmt_conf_array);
int need_trsmt(const char *service_code);
int transmit(char *trans_buffer, int sn_in_G_trsmt_conf, int is_rec);

#endif
