/*
*wifi_demo.c    wifi operation demo
*/
#include <stdint.h>
#include "wifi.h"
#include "wifi_netif.h"
#include "moal_wifi.h"
#include "wifi_backup_common.h"
#include "command.h"
#include "ak_common.h"
#include "ak_thread.h"
#include "ak_partition.h"
#include "akos_error.h"
#include "lwip/ip_addr.h"
#include "dev_info.h"
#include "drv_gpio.h"
#include "ak_ini.h"
#include "mlanutl.h"
#include "atbm_wifi_driver_api.h"

#include "hal_timer.h"

/********************************************************
*			 Macro
********************************************************/

#define SDIO_BACK_PARTI          "SDIOPA"
//#define SERVER_IP_PARTI			 "SERVER"


/******************************************************
*                    Constant         
******************************************************/


/******************************************************
*                    Type Definitions         
******************************************************/

/** Character, 1 byte */
typedef signed char t_s8;
/** Unsigned character, 1 byte */
typedef unsigned char t_u8;

/** Short integer */
typedef signed short t_s16;
/** Unsigned short integer */
typedef unsigned short t_u16;

/** Integer */
typedef signed int t_s32;
/** Unsigned integer */
typedef unsigned int t_u32;

/** Long long integer */
typedef signed long long t_s64;
/** Unsigned long long integer */
typedef unsigned long long t_u64;

/** Type definition of mlan_ds_ps_cfg for MLAN_OID_PM_CFG_PS_CFG */
typedef struct _mlan_ds_ps_cfg {
    /** PS null interval in seconds */
	t_u32 ps_null_interval;
    /** Multiple DTIM interval */
	t_u32 multiple_dtim_interval;
    /** Listen interval */
	t_u32 listen_interval;
    /** Adhoc awake period */
	t_u32 adhoc_awake_period;
    /** Beacon miss timeout in milliseconds */
	t_u32 bcn_miss_timeout;
    /** Delay to PS in milliseconds */
	t_s32 delay_to_ps;
    /** PS mode */
	t_u32 ps_mode;
} mlan_ds_ps_cfg, *pmlan_ds_ps_cfg;

typedef struct _mlan_ds_auto_assoc
{
	/*1/2/3 driver auto assoc/driver auto re-connect/FW auto re-connect		                   
	auto assoc takes effect in new connection (e.g. iwconfig essid),		                   
	driver will auto retry if association failed;		                   
	auto re-connect takes effect when link lost, driver/FW will try		                   
	to connect to the same AP
	*/
	t_u8 auto_assoc_type;
	/*1/0 on/off*/
	t_u8 auto_assoc_enable;
	/*0x1-0xff The value 0xff means retry forever (default 0xff)*/
	t_u8 auto_assoc_retry_count;
	/*0x0-0xff Time gap in seconds (default 10)*/
	t_u8 auto_assoc_retry_interval;
	/*: Bit 0:		                   
	Set to 1: Firmware should report link-loss to host if AP rejects	
	authentication/association while reconnecting		                  
	Set to 0: Default behavior: Firmware does not report link-loss		                             
	to host on AP rejection and continues internally		                   
	Bit 1-15: Reserved		                  
	The parameter flag is only used for FW auto re-connect
	*/
	t_u8 auto_assoc_flags;
}mlan_ds_auto_assoc, *pmlan_ds_auto_assoc;

typedef struct _mlan_ds_smc_chan_list {
	/** channel number*/
	t_u8 chan_number;
	/** channel width 0: 20M, 1: 10M, 2: 40M, 3: 80Mhz*/
	t_u8 chanwidth;
	/** min scan time */
	t_u16 minScanTime;
	/**max scan time */
	t_u16 maxScanTime;
} mlan_ds_smc_chan_list;
typedef struct _mlan_ds_smc_frame_filter {
	/** destination type */
	t_u8 destType;
	/** Frame type */
	t_u8 frameType;
} mlan_ds_smc_frame_filter;
typedef struct _mlan_ds_smc_mac_addr {
	/** start address */
	t_u8 startaddr[MLAN_MAC_ADDR_LENGTH];
	/**end address */
	t_u8 endaddr[MLAN_MAC_ADDR_LENGTH];
	/** filter type */
	t_u16 filterType;
} mlan_ds_smc_mac_addr;
typedef struct _mlan_ds_smc {
	/** smc action, 0: get, 1: set, 2: start, 3: stop */
	t_u16 action;
	/** channel list number*/
	t_u8 chan_num;
	/**channel list*/
	mlan_ds_smc_chan_list chanlist[MAX_CHANNEL_LIST];
	/** frame filter set*/
	t_u8 filterCount;
	/**frame filter */
	mlan_ds_smc_frame_filter framefilter[MAX_FRAME_FILTER];
	/** address range set */
	t_u8 addrCount;
	/** multi address */
	mlan_ds_smc_mac_addr addr[MAX_ADDR_FILTER];
	/** ssid length */
	t_u8 ssid_len;
	/** ssid */
	t_u8 ssid[32];
	/** beacon period */
	t_u16 beaconPeriod;
	/** beacon of channel */
	t_u8 beaconOfChan;
	/** probe of channel */
	t_u8 probeOfChan;
	/** customer ie */
	t_u8 customer_ie[MAX_IE_SIZE];
	/** IE len */
	t_u8 ie_len;
} mlan_ds_smc;


typedef struct _mlan_ds_misc_keep_alive {
	t_u8 mkeep_alive_id;
	t_u8 enable;
	t_u8 start;
	t_u16 send_interval;
	t_u16 retry_interval;
	t_u16 retry_count;
	t_u8 dst_mac[MLAN_MAC_ADDR_LENGTH];
	t_u8 src_mac[MLAN_MAC_ADDR_LENGTH];
	t_u16 pkt_len;
	t_u8 packet[MKEEP_ALIVE_IP_PKT_MAX];
} mlan_ds_misc_keep_alive;


/** mlan_ioctl_req data structure */
typedef struct _user_ioctl_req
{
    /** Request id */
    t_u32 req_id;
    /** Action: set or get */
    t_u32 action;
    union
	{
		/** Power saving mode for MLAN_OID_PM_CFG_IEEE_PS */
		t_u8 ps_mode;
		/** ps config mlan_ds_ps_cfg for MLAN_OID_PM_CFG_PS_CFG */
        mlan_ds_ps_cfg ps_cfg;
		/*fast link*/
		t_u8 fast_link_enable;
		/** Host Sleep configuration for MLAN_OID_PM_CFG_HS_CFG */
		mlan_ds_hs_cfg hs_cfg;
		/** fw re-connect cfg param set */
		mlan_ds_auto_assoc auto_reconnect;
		mlan_ds_smc smc;
		mlan_ds_misc_keep_alive keep_alive;
    }param;
} user_ioctl_req, *puser_ioctl_req;


typedef struct _wifi_cmd_entry
{
    char cmd_name[32];
    T_pfCMD cmd_entry;
}wifi_cmd_entry_t;
 
/******************************************************
*                    Global Variables         
******************************************************/
void* cfg_handle;

/******************************************************
*			 Local Variables
******************************************************/
static char *help_ap[]={
	"wifi ap tools\n",
	"usage:ap start [ch][mode: 0 bg, 1 bgn] [ssid] <security_type: 0 open, 1 WEP, 2 WPA, 3 WPA2> <password> \n"
	"	   ap stop\n"
};
static char *help_sta[]={
	"wifi sta tools\n",
	"usage: sta conn [ssid] <password>\n"
	"		sta disconn\n"
	"		sta status, get connection status\n"
	"		sta config [ssid] [password] [IP] [port]\n"
	"		sta clearcfg, clear server config\n"
	"		sta fastlink\n"
	"		sta hscfg [condigion] [gpio] [gap] [ind_gpio] [level] \n"
	"		sta mefcfg, set mef with default\n"
	"		sta keepalive start/stop\n"
	"		sta dhcp, get ip by dhcp\n"
	"		sta clearip, clear ip in flash\n"
	"		sta sleep\n"
	"		sta wakeup\n"
	"		sta init\n"
	"		sta scan\n"
};
static char *help_sdio[]={
	"sdio back test\n",
	"usage: sdio resetflag 0/1\n"
	"		sdio back  \n"
	"		sdio restore \n"
			
};

static int sta_reconn_flag = AK_TRUE;
static ak_pthread_t  g_sta_conn_thread_id = AK_INVALID_TASK;
static struct _conn_param conn_param;

  
/******************************************************
*                    Function prototype                           
******************************************************/
static int sdio_flag_set(uint32_t reset_flag);
static int sdio_flag_get(uint32_t *reset_flag);
extern char wifi_get_init_status(void);
extern char wifi_init_check();

/******************************************************
*		        Local Function Declarations
******************************************************/

/*keep  connection thread*/
static void sta_conn_thread(void *args)
{
	struct _conn_param *conn_param = (struct _conn_param*)args;
	
	while(sta_reconn_flag == AK_TRUE)
	{
		if (!wifi_isconnected())
		{	
			ak_print_normal("Connect to SSID: < %s >  with password:%s\n", conn_param->essid,  conn_param->password);
			wifi_connect(conn_param->essid, conn_param->password);
			if (wifi_isconnected())
			{
				ak_print_normal("wifi Association ok\n");
				//wifi_power_cfg(0); //wmj for test
				wifistation_netif_init();
                wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_EVERY,NULL);
                wifistation_netif_get_ip();
				//break;				
			}
		}
		ak_sleep_ms(1000);
	}
	ak_print_normal("sta conn thread exit.\n");
	ak_thread_exit();
}


/******************************************************
*		        Local Function Declarations
******************************************************/
// add by charlie for quick connect

/*keep quick connection thread*/
//extern void wifi_get_fast_connect_info(uint8_t *channel, uint8_t *pmk);
extern int wifi_get_fast_connect_info(struct bss_descriptor *bss_saved);

static void sta_quick_conn_thread(void *args)
{
	int connecting = 0;
	struct _conn_param *conn_param = (struct _conn_param*)args;

	struct wifi_info wifi_info_saved;	
    memset(&wifi_info_saved, 0, sizeof(struct wifi_info));

    wifi_backup_usr_data(0, &wifi_info_saved,  sizeof(wifi_info_saved));
		
	while(sta_reconn_flag == AK_TRUE)
	{
		if (!wifi_isconnected() && !connecting)
		{
			ak_print_normal("Connect to SSID: < %s >  with password:%s\n", conn_param->essid,  conn_param->password);
			wifi_quick_connect(conn_param->essid, conn_param->password, &wifi_info_saved.bss_descriptor);
			connecting = 1;
		}else if(wifi_isconnected()){
			ak_print_normal("wifi Association ok\n");
			wifi_get_fast_connect_info(&wifi_info_saved.bss_descriptor);
			wifi_backup_usr_data(1, &wifi_info_saved,  sizeof(wifi_info_saved));
			wifistation_netif_init();
//	        wifistation_netif_set_dhcp(DHCP_MODE_STATIC, "192.168.43.43");
			wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_FIRST, NULL);
//			wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_EVERY, NULL);
	        wifistation_netif_get_ip();
			break;
		}
		ak_sleep_ms(50);
	}
	ak_print_normal("sta conn thread exit.\n");
	ak_thread_exit();
}

/**
 * get configuration for camview (including ssid password ip port and port).
 *
 * @param ip_info the network interface if infomation including ip gw netmask
 */

int wifi_set_config(struct server_info *t_server_info)
{
	void *handle = NULL;
	char str_buf[30]={0};
	int ret = -1;
#if 0
	char *server_parti = SERVER_IP_PARTI;

	ak_print_normal("open %s \n", server_parti);
	
	handle = ak_partition_open(server_parti);
	if(handle == NULL)
	{
		ak_print_normal("open %s error\n", server_parti);
	}
	else
	{
		ret = ak_partition_write(handle, (char *)t_server_info, sizeof(struct server_info));
		if(ret < 0)
		{
			ak_print_normal("write %s error\n", server_parti);
		}
		else
		{
			ak_print_normal("back IP info on %s OK\n", server_parti);
		}
		ak_partition_close(handle);
	}
#endif
	ret=ak_ini_set_item_value(cfg_handle,"SERVER","essid",(char *)t_server_info->essid);
	ret=ak_ini_set_item_value(cfg_handle,"SERVER","pswd",(char *)t_server_info->password);

	sprintf(str_buf,"%d",t_server_info->server_ip);
	ret=ak_ini_set_item_value(cfg_handle,"SERVER","ip",str_buf);
	memset(str_buf,0,sizeof(str_buf));
	
	sprintf(str_buf,"%d",t_server_info->server_port);
	ret=ak_ini_set_item_value(cfg_handle,"SERVER","port",str_buf);
	memset(str_buf,0,sizeof(str_buf));
	return ret;	
}


/**
 * get configuration for camview (including ssid password ip port and port).
 *
 * @param ip_info the network interface if infomation including ip gw netmask
 */

int wifi_get_config(struct server_info *t_server_info)
{
	int ret = -1;
	char str_buf[30]={0};
#if 0
	void *handle = NULL;
	char *server_parti = SERVER_IP_PARTI;

	
	ak_print_normal("open %s \n", server_parti);
	
	handle = ak_partition_open(server_parti);
	if(handle == NULL)
	{
		ak_print_normal("open %s error\n", server_parti);
	}
	else
	{
		ret = ak_partition_read(handle, (char *)t_server_info, sizeof(struct server_info));
		if(ret < 0)
		{
			ak_print_normal("write %s error\n", server_parti);
		}
		else
		{
			ak_print_normal("back IP info on %s OK\n", server_parti);
		}
		ak_partition_close(handle);
	}
#endif
	memset(t_server_info->essid,0,sizeof(t_server_info->essid));
	ret=ak_ini_get_item_value(cfg_handle,"SERVER","essid",t_server_info->essid);
	
	memset(t_server_info->password,0,sizeof(t_server_info->password));
	ret=ak_ini_get_item_value(cfg_handle,"SERVER","pswd",t_server_info->password);

	ret=ak_ini_get_item_value(cfg_handle,"SERVER","ip",str_buf);
	t_server_info->server_ip=atoi(str_buf);
	memset(str_buf,0,sizeof(str_buf));

	
	ret=ak_ini_get_item_value(cfg_handle,"SERVER","port",str_buf);
	t_server_info->server_port=atoi(str_buf);
	memset(str_buf,0,sizeof(str_buf));
	
	return ret;	
}


static unsigned char magic_mef_conf[] = 
	"mefcfg={\n"\
		"Criteria=3\n"\
		"NumEntries=1\n"\
		"mef_entry_0={\n"\
			"mode=1\n"\
			"action=3\n"\
			"filter_num=3\n"\
			"RPN=Filter_0 && Filter_1 || Filter_2\n"\
			"Filter_0={\n"\
				"type=0x42\n"\
				"pattern=80\n"\
				"offset=44\n"\
				"numbyte=2\n"\
			"}\n"\
			"Filter_1={\n"\
				"type=0x41\n"\
				"repeat=1\n"\
				"byte=c0:a8:01:68\n"\
				"offset=34\n"\
			"}\n"\
			"Filter_2={\n"\
				"type=0x41\n"\
				"repeat=16\n"\
				"byte=00:50:43:21:be:14\n"\
				"offset=56\n"\
			"}\n"\
		"}\n"\
	"}\n";

/*****************************************
 * @brief wifi connect function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
#ifdef WFII_MLANUTL
extern int process_aggrpriotbl(int argc, char *argv[]);
extern int process_addbareject(int argc, char *argv[]);
extern int process_memrdwr(int argc, char *argv[]);


//wmm para,use for improve the competitive power of occupy wifi channel 
static wifi_wmm_para_t wmm_para=
{
	//BE para
	.aci0 = 0,
	.aifsn0 = 2,
	.ecwmax0 = 3,
	.ecwmin0 = 2,
	.txop0 = 150,

	//BK para
	.aci1 =1,
	.aifsn1 = 2,
	.ecwmax1 = 3,
	.ecwmin1 = 2,
	.txop1 = 150,

	//VI para
	.aci2 =2,
	.aifsn2 = 2,
	.ecwmax2 = 2,
	.ecwmin2 = 1,
	.txop2 = 360,

	//VO para
	.aci3 =3,
	.aifsn3 = 2,
	.ecwmax3 = 3,
	.ecwmin3 = 2,
	.txop3 = 150
};							//wmm para,use for improve the competitive power of occupy wifi channel 
#endif

static void wifi_sta_testcontime(int argc, char **args)
{
	int ret = -1, connecting = 0, t1, t2;
    if (argc != 2 && argc != 3)
    {
        ak_print_normal("%s",help_sta[1]);
        return;
    }
    if(strlen(args[1]) > MAX_SSID_LEN)
    {
        ak_print_normal("ssid should less than 32 characters\n");
        return;
    }
    if(argc == 3 && strlen(args[2]) > MAX_KEY_LEN)
    {
        ak_print_normal("password should less than 64 characters\n");
        return;
    }
    
    strcpy(conn_param.essid, args[1]);
    
    if(argc == 3)
    {
        strcpy(conn_param.password, args[2]);
    }
    else
    {
        memset(conn_param.password, 0, MAX_KEY_LEN);
    }
	
	struct wifi_info wifi_info_saved;	
    memset(&wifi_info_saved, 0, sizeof(struct wifi_info));

    wifi_backup_usr_data(0, &wifi_info_saved,  sizeof(wifi_info_saved));
	t1 = get_tick_count();
	ak_print_normal(" jkstart init wifi\n");//,%ld\n", get_tick_count());
	wifi_init(0);
	ak_print_normal(" jkstart init wifi ok\n");//,%ld\n", get_tick_count());
	wifi_set_mode(WIFI_MODE_STA);
	ak_print_normal(" jk set mode ok\n");
	while(sta_reconn_flag == AK_TRUE)
	{
		if (!wifi_isconnected() && !connecting)
		{
			ak_print_normal("jk Connect to SSID: < %s >  with password:%s\n", conn_param.essid,  conn_param.password);
			wifi_quick_connect(conn_param.essid, conn_param.password, &wifi_info_saved.bss_descriptor);
			connecting = 1;
		}else if(wifi_isconnected()){
			t2 = get_tick_count();
			ak_print_normal("jk wifi Association ok,t:%ld\n",t2-t1);
			wifi_get_fast_connect_info(&wifi_info_saved.bss_descriptor);
			wifi_backup_usr_data(1, &wifi_info_saved,  sizeof(wifi_info_saved));
			wifistation_netif_init();
//	        wifistation_netif_set_dhcp(DHCP_MODE_STATIC, "192.168.43.43");
			wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_FIRST, NULL);
//			wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_EVERY, NULL);
	        wifistation_netif_get_ip();
			break;
		}
		ak_sleep_ms(10);
	}
	
    //wifi_connect(conn_param.essid,conn_param.password);
}

static void wifi_sta_conn(int argc, char **args)
{
    int ret = -1;
    if (argc != 2 && argc != 3)
    {
        ak_print_normal("%s",help_sta[1]);
        return;
    }
    if(strlen(args[1]) > MAX_SSID_LEN)
    {
        ak_print_normal("ssid should less than 32 characters\n");
        return;
    }
    if(argc == 3 && strlen(args[2]) > MAX_KEY_LEN)
    {
        ak_print_normal("password should less than 64 characters\n");
        return;
    }
    
    strcpy(conn_param.essid, args[1]);
    
    if(argc == 3)
    {
        strcpy(conn_param.password, args[2]);
    }
    else
    {
        memset(conn_param.password, 0, MAX_KEY_LEN);
    }

   // wifi_init(0);
#ifdef WFII_MLANUTL
	process_param_config(23, &wmm_para);
	process_aggrpriotbl(3, NULL);

	process_aggrpriotbl(2, NULL);
#if 0
	char *argv1[10];
	argv1[0] = "mlanutl";
	argv1[1] = "mlan0";
	argv1[2] = "addbareject1";
	argv1[3] = "1";
	argv1[4] = "1";
	argv1[5] = "1";
	argv1[6] = "1";
	argv1[7] = "1";
	argv1[8] = "1";
	argv1[9] = "1";
	argv1[10] = "1";
	//process_addbareject(10, argv1);
	process_addbareject(11, NULL);
	process_addbareject(2, NULL);

	process_memrdwr(3, NULL);
	//process_memrdwr(4, NULL);
	//process_memrdwr(3, NULL);
#endif
#endif
#if 0
    wifi_connect(conn_param.essid,conn_param.password);
#else
    /*create a task to connetc AP,  so the STA can reconnect AP case the AP unavailable temporily for some reason*/
    if(g_sta_conn_thread_id == AK_INVALID_TASK)
    {
        ret = wifi_set_mode(WIFI_MODE_STA);
//		ak_sleep_ms(100);
        if(ret == -1)
            return;
            
        sta_reconn_flag = AK_TRUE;
        ak_thread_create(&g_sta_conn_thread_id , (void*)sta_quick_conn_thread , &conn_param, 4096, 10);
    }
    else
    {
        ak_print_normal("sta is connecting, please disconnect it first\n");
    }
#endif
    
}

/*****************************************
 * @brief wifi disconnect function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_disconn(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("%s",help_sta[1]);
		return;
	}

	ak_print_normal("disconnect wifi \n");
	
	sta_reconn_flag = AK_FALSE;
	wifi_disconnect();
	if(g_sta_conn_thread_id != AK_INVALID_TASK)
	{
		ak_thread_join(g_sta_conn_thread_id);
		g_sta_conn_thread_id = AK_INVALID_TASK;
	}
	
}

static void wifi_sta_status(int argc, char **args)
{
	if(wifi_isconnected())
	{
		ak_print_normal("wifi connected\n");
	}
	else
	{
		ak_print_normal("wifi disconnected\n");
	}
}    

/*****************************************
 * @brief set wifi info config
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_info_config(int argc, char **args)
{
	unsigned long  ipaddr = IPADDR_NONE;
	unsigned short port = 0;

	struct server_info t_server_info;
	memset(&t_server_info, 0, sizeof(struct ip_info));
	ak_print_notice("start config!\n");

	if(argc == 1)
	{
		wifi_get_config(&t_server_info);
		ak_print_normal("ssid: %s\n", t_server_info.essid);
		ak_print_normal("password: %s\n", t_server_info.password);
		ak_print_normal("IP: %u.%u.%u.%u\n", 
			t_server_info.server_ip & 0xff, 
			(t_server_info.server_ip >> 8) & 0xff,
			(t_server_info.server_ip >> 16) & 0xff, 
			(t_server_info.server_ip >> 24) & 0xff);
		ak_print_normal("port: %d\n", t_server_info.server_port);
		return;
	}
	if(argc != 5)
	{
		ak_print_normal("%s", help_sta[1]);
		return;
	}
	//SSID
	strcpy(t_server_info.essid, args[1]);
	//password
	strcpy(t_server_info.password, args[2]);
	//ipaddr
	
    ipaddr = inet_addr((char *)args[3]);
	if (IPADDR_NONE == ipaddr)
	{
	   ak_print_error("set remote_ipaddr wrong.\n");
	   return;
	}
	t_server_info.server_ip = ipaddr;
	
	port = atoi(args[4]);
	if(port > 65535)
	{
		ak_print_error("port should less than 65535\n");
		return;
	}
	t_server_info.server_port = port;
	
	wifi_set_config(&t_server_info);
}

static void wifi_config_clear(int argc, char **args)
{
    struct server_info t_server_info;
    memset(&t_server_info, 0, sizeof(struct server_info));
    wifi_set_config(&t_server_info);
}

/*****************************************
 * @brief dhcp function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_dhcp(int argc, char **args)
{
    if (!wifi_isconnected())
    {
        ak_print_normal("please connect to ap first\n");
        return;
    }   
        
    wifistation_netif_init();
    wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_EVERY,NULL);
    wifistation_netif_get_ip();
}

extern int prepare_buffer(t_u8 * buffer, t_s8 * cmd, t_u32 num, char *args[]);
extern int process_host_cmd_resp(char *cmd_name, t_u8 * buf);


/*****************************************
 * @brief wifi driver init test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_init(int argc, char **args)
{
    t_u8 resume;
    
    if(argc == 2)
    {
        wifi_init(1);
    }
    else
    {
        wifi_init(0);
    }    
}

static void wifi_sta_wakeup(int argc, char **args)
{
    wifi_wakeup();
}

static void wifi_sta_sleep(int argc, char **args)
{
    wifi_sleep(5,30);
}

/*****************************************
 * @brief wifi keepalive function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_keepalive(int argc, char **args)
{
    int ret = -1;
	keep_alive_t keep_alive;
	
	if(strcmp(args[1], "start") == 0)
	{
		keep_alive.enable = 1;
        
	    if(args[2] != NULL)
            keep_alive.dst_ip = inet_addr(args[2]);
        else
            keep_alive.dst_ip = inet_addr("192.168.1.231");
        
        if(args[3] != NULL)
		    keep_alive.dst_port = atoi(args[3]);
        else
            keep_alive.dst_port = 5006;
		keep_alive.send_interval = 10; //10 seconds
		keep_alive.retry_interval = 3; //3 sconds
		keep_alive.retry_count = 5;
		strcpy(keep_alive.payload, "keepalive");
		keep_alive.payload_len = strlen("keepalive");
	}
	else if(strcmp(args[1], "stop") == 0)
	{
		keep_alive.enable = 0;
        ret = wifi_wakeup();
        if(!ret)        
            ak_print_notice(" wifi keepalive stop ok\n");
        else
            ak_print_error(" wifi keepalive stop fail \n");
        return;
	}
	else
	{
		ak_print_normal("%s",help_sta[1]);
		return;
	}
	
	wifi_keepalive_set(&keep_alive);

    ret = wifi_sleep();
    if(!ret)        
        ak_print_notice(" wifi_sleep ok\n");
    else
        ak_print_error(" wifi_sleep fail \n");  
}

/*
* wildcast ssid in ap_list table
* return: 1 match a ssid, 0 no ssid matched 
*/
static int wifi_match_ssid(unsigned char *in_ssid, unsigned char *out_ssid, wifi_ap_list_t *ap_list)
{
	int ssid_len = strlen(in_ssid);
	int i;
	for(i = 0; i < ap_list->ap_count; i++)
	{
		if(strncmp(in_ssid, ap_list->ap_info[i].ssid, ssid_len) == 0)
		{
			ak_print_normal("match ssid:%s, channel:%d, rssi:%d ,security:%d\n", 
            ap_list->ap_info[i].ssid,ap_list->ap_info[i].channel, ap_list->ap_info[i].rssi,ap_list->ap_info[i].security);
            
			strcpy(out_ssid, ap_list->ap_info[i].ssid);
			return 1;
		}
	}
    ak_print_normal("can not find %s, pls try again\n", in_ssid);
	return 0;
}


/*****************************************
 * @brief wifi keepalive function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_scan_demo(int argc, char **args)
{
    wifi_ap_list_t ap_list;
    int ret, scan_cnt = 5;
    unsigned char out_ssid[MAX_SSID_LEN];

    //check wifi driver init status
    if(!wifi_init_check())
        //ret = wifi_init(0);
    
    if(ret != 0)
        return;

    wifi_set_mode(WIFI_MODE_STA);
	ak_sleep_ms(50);
    do
    {           
        wifi_scan(&ap_list);

        if(argc == 2)
        {
            if( wifi_match_ssid(args[1], out_ssid, &ap_list) == 1)
                break;

            scan_cnt--;
            ak_sleep_ms(1000);
        }
        else
            return;        
    }while(scan_cnt > 0);

    return;
}
static void wifi_start_log(int argc, char **args)
{
	if(strcmp(args[1], "1") == 0)
	{
		_wifi_start_log(1);
	}
	else if(strcmp(args[1], "0") == 0)
	{
		_wifi_start_log(0);
	}

}
static void sdio_dump(int argc, char **args){

	atbm_sdio_dump();

}
#if 0

static void wifi_wps_demo(int argc, char **args)
{
	int ret;
	struct bss_descriptor bss_connected;
	ret =  wifi_init(0);
#ifdef WIFI_MLANUTL
	process_param_config(23, &wmm_para);
	process_aggrpriotbl(3, NULL);
	process_aggrpriotbl(2, NULL);
	
	process_addbareject(11, NULL);
	process_addbareject(2, NULL);
#endif
    if(ret != 0)
        return;
	wifi_set_mode(WIFI_MODE_STA);
	ret = wifi_wps_connect(&bss_connected);
	if(ret == 0)
	{
		wifistation_netif_init();
    	wifistation_netif_set_dhcp(DHCP_MODE_DYNAMIC_EVERY,NULL);
    	wifistation_netif_get_ip();
	}
}
#endif

#if DRIVER_TEST
extern int moal_wifi_set_txpower(uint8_t dbm);
extern int wifi_set_pscfg(power_save_t  *pscfg);
extern int wifi_get_pscfg(power_save_t  *pscfg);
extern int moal_wifi_get_fastlink_enable(void);
extern int moal_wifi_get_hscfg(mlan_ds_hs_cfg *hscfg);
extern int moal_wifi_set_hscfg(mlan_ds_hs_cfg *hscfg);
extern int moal_wifi_set_fastlink_enable(unsigned char fastlink_enable);
extern int moal_wifi_set_auto_reconnect(mlan_ds_auto_assoc *auto_reconnect);
extern int moal_wifi_get_pscfg(power_save_t *pscfg);
extern int moal_wifi_get_auto_reconnect(mlan_ds_auto_assoc *auto_reconnect);

/*****************************************
 * @brief sta fast link function test
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_fastlink(int argc, char **args)
{
    int fastlink;
        
    //sdio_flag_set(1);
    //sdio_backup(SDIO_BACK_PARTI);
    if(argc == 1)
    {
        fastlink = moal_wifi_get_fastlink_enable();
        ak_print_normal("fast link status %d\n", fastlink);
    }
    else
    {
        if(strcmp(args[1], "1") == 0)
        {
            moal_wifi_set_fastlink_enable(1);
        }
        else if(strcmp(args[1], "0") == 0)
        {
            moal_wifi_set_fastlink_enable(0);
        }
        else
        {
            ak_print_normal("wrong args\n");
        }
    } 
}

/*****************************************
 * @brief sta host config set
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_hscfg(int argc, char **args)
{
    mlan_ds_hs_cfg hscfg;
    /* hs config*/
    if(argc == 1)/*get*/
    {
        moal_wifi_get_hscfg(&hscfg);
        /* GET operation */
        ak_print_normal("HS Configuration:\n");
        ak_print_normal("  Conditions: %d\n", (int)hscfg.conditions);
        ak_print_normal("  GPIO: %d\n", (int)hscfg.gpio);
        ak_print_normal("  GAP: %d\n", (int)hscfg.gap);
        ak_print_normal("  Indication GPIO: %d\n", (int)hscfg.ind_gpio);
        ak_print_normal("  Level for normal wakeup: %d\n", (int)hscfg.level);
        
    }
    else if(argc == 4 )
    {
        hscfg.is_invoke_hostcmd = MTRUE;
        hscfg.conditions = atoi(args[1]);
        hscfg.gpio = atoi(args[2]);
        hscfg.gap = atoi(args[3]);
        hscfg.hs_wake_interval = 0;
        hscfg.ind_gpio = 0;
        hscfg.level = 0;
        moal_wifi_set_hscfg(&hscfg);
    }        
}

static void wifi_sta_ps(int argc, char **args)
{
    int ps_enable;
    if(argc == 2)
    {
        ps_enable = atoi(args[1]);
        ak_print_normal("set sta ps %d\n", ps_enable);
        wifi_power_cfg(ps_enable);
    }
    else
    {
        ak_print_normal(" sta ps 0/1\n");
    }
}

/*****************************************
 * @brief sta power save config set
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_pscfg(int argc, char **args)
{
    power_save_t pscfg;
    if(argc == 1)/*get*/
    {
        moal_wifi_get_pscfg(&pscfg);
        ak_print_normal("PS Configuration:\n");
        ak_print_normal("  ps_null_interval: %d\n", (int)pscfg.ps_null_interval);
        ak_print_normal("  multiple_dtim_interval: %d\n", (int)pscfg.dtim_interval);
        ak_print_normal("  delay_to_ps: %d\n", (int)pscfg.delay_to_ps);
        ak_print_normal("  ps_mode: %d\n", (int)pscfg.ps_mode);
    }
    else if(argc > 1)
    {
        
        pscfg.ps_null_interval = atoi(args[1]);
        pscfg.dtim_interval = 3;
        pscfg.delay_to_ps = 500;
        pscfg.ps_mode = 1;
        
        if(argc > 2)
        {
            if(!strncmp(args[2], "0x", 2)) 
            {
                pscfg.dtim_interval = strtol(args[2], NULL, 16);
            }
            else
            {
                pscfg.dtim_interval = atoi(args[2]);
            }
        }
        if(argc > 3)
        {
            pscfg.delay_to_ps = atoi(args[3]);
        }
        if(argc > 4)
        {
            pscfg.ps_mode = atoi(args[4]);
        }

        wifi_set_pscfg(&pscfg);    
    }
}

/*****************************************
 * @brief sta auto_reconnect set
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_sta_reconnect(int argc, char **args)
{
	mlan_ds_auto_assoc auto_reconnect;

    if(argc == 1)/*option get*/
    {
        moal_wifi_get_auto_reconnect(&auto_reconnect);
        
    }
    else
    {
        if(strcmp(args[1], "1") == 0)
        {
            auto_reconnect.auto_assoc_enable = 1;
        }
        else if(strcmp(args[1], "0") == 0)
        {
            auto_reconnect.auto_assoc_enable = 0;
        }
        else
        {
            ak_print_normal("wrong args\n");
            return;
        }
        auto_reconnect.auto_assoc_type = 3;
        auto_reconnect.auto_assoc_retry_count = 0xff;
        auto_reconnect.auto_assoc_retry_interval= 10; /* 10 second*/
        auto_reconnect.auto_assoc_flags = 0;
        
        moal_wifi_set_auto_reconnect(&auto_reconnect);
    }
}

static void wifi_sta_txpower(int argc, char **args)
{
    if(argc == 2)
    {
        uint8_t dbm = atoi(args[1]);
        if(dbm > 0 && dbm < 18)
        {
            moal_wifi_set_txpower(dbm);
        }
    }
    else
    {
        ak_print_normal("argument error\n");
    }
}
#endif

extern int wifi_smartconfig();

static void wifi_sta_smartconfig(int argc, char **args)
{
	wifi_set_mode(WIFI_MODE_STA);
	ak_sleep_ms(50);
	wifi_smartconfig();
	ak_print_normal("sta smartconfig  over\n");
	
}

extern int wifi_etf_start_tx(int channel,int rate_value,int is_40M, int greedfiled);

extern int wifi_etf_stop_tx();

extern int wifi_etf_start_rx(int channel ,int is_40M);

extern int wifi_etf_stop_rx();

extern int wifi_mfg_CarrierTone(WLAN_CHANNEL channel);

static void wifi_StartTx(int argc, char **args){
	t_u32 uiChannel, uiRate, is_40M, uiGreenfield;
	t_s32 flag = 0, i = 0, j = 0;
	char uiRatePtr[4] = {0};
	if(argc < 5){
		ak_print_normal("need more parameter,please try again!, %d\n", argc);
		return ;
	}
	uiChannel = atoi(args[1]);

	while(args[2][j] != '\0'){
		if(args[2][j] == '.'){
			flag = 1;
		}else{
			uiRatePtr[i++] = args[2][j];
		}
		j++;
	}

	uiRate = flag? atoi(uiRatePtr) : atoi(uiRatePtr) * 10;
	is_40M = atoi(args[3]);
	uiGreenfield = atoi(args[4]);
	ak_print_normal("Ch : %d, Rate : %d, is_40M : %d greenfiled: %d\n", uiChannel, uiRate, is_40M, uiGreenfield);
	wifi_set_mode(WIFI_MODE_STA);
	ak_sleep_ms(50);
	if(wifi_etf_start_tx(uiChannel, uiRate, is_40M/*40M*/, uiGreenfield/*greedfiled*/) < 0){
		ak_print_normal("start tx failed!\n");
	}
}

static void wifi_StopTx(int argc, char **args){
	wifi_etf_stop_tx();
}

static void wifi_StartRx(int argc, char **args){
	t_u32 uiChannel, is_40M;
	if(argc < 4){
		ak_print_normal("need more parameter,please try again!\n");
	}
	uiChannel = atoi(args[2]);
	is_40M = atoi(args[3]);
	ak_print_normal("Ch : %d is_40M: %d\n", uiChannel, is_40M);
	if(wifi_etf_start_rx(uiChannel, is_40M/*40M*/) < 0){
		ak_print_normal("start etf rx failed!\n");
	}
}

static void wifi_StopRx(int argc, char **args){
	wifi_etf_stop_rx();
}

#if 0
static void wifi_StartCCAMeas(int argc, char **args){
	atbmwifi_enable_lmaclog(1);
	wsm_write_mib(&g_hw_prv, WSM_MIB_ID_FW_CMD,"ccastart ", 9, g_vmac->if_id);
}

static void wifi_StopCCAMeas(int argc, char **args){
	atbmwifi_enable_lmaclog(0);
	wsm_write_mib(&g_hw_prv, WSM_MIB_ID_FW_CMD,"ccastop ", 8, g_vmac->if_id);
}
#endif
static void wifi_MFGCarrierTone(int argc, char **args)
{
    t_u32 uiChannel;
    t_s32  iError;

    uiChannel = atoi(args[1]);

    ak_print_normal("Ch : %d \n", uiChannel);

    iError = wifi_mfg_CarrierTone(uiChannel);

    ak_print_normal("CarrierTone Status : %d \n", iError);
}

/*****************************************
 * @brief start wifi ap
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_ap_start(int argc, char **args)
{
    struct _apcfg  ap_cfg;
    int ret = -1;
    
    if (argc < 4)
    {
        ak_print_normal("%s",help_ap[1]);
        return;
    }

    memset(&ap_cfg, 0, sizeof(struct _apcfg));
   
    ap_cfg.channel = atoi(args[1]);
    if(ap_cfg.channel < 0 || ap_cfg.channel > 14)
    {
        ak_print_normal("channel should be 1 ~ 14\n");
        return;
    }
    
    ap_cfg.mode = atoi(args[2]); 
    //0:bg mode  1:bgn mode
    
    strcpy(ap_cfg.ssid, args[3]);
    ap_cfg.ssid_len=strlen(ap_cfg.ssid);
    if(strlen(ap_cfg.ssid) > MAX_SSID_LEN)
    {
        ak_print_normal("AP SSID should be less than 32 characters \n");
        return;
    }

    if (argc >= 5)
    {
        ap_cfg.enc_protocol = atoi(args[4]);
        if(ap_cfg.enc_protocol >= KEY_MAX_VALUE)
        {
            ak_print_normal(" wrong security_type \n %s",help_ap[1]);
            return;
        }
        
        if((ap_cfg.enc_protocol != KEY_NONE) && argc == 5)
        {
            ak_print_normal(" please input password \n %s",help_ap[1]);
            return;
        }
    }
    
    if (argc == 6)
    {
        if(strlen(args[5]) < MIN_KEY_LEN || strlen(args[5]) > MAX_KEY_LEN)
        {
            ak_print_normal("AP key should be %d ~ %d characters \n", MIN_KEY_LEN, MAX_KEY_LEN);
            return;
        }   
        strcpy(ap_cfg.key, args[5]);
	ap_cfg.key_len=strlen(ap_cfg.key);
    }

    //if wifi is in station mode,we should disconnect it
    if(g_sta_conn_thread_id != AK_INVALID_TASK)
    {   
        sta_reconn_flag = AK_FALSE;
 //       wifi_disconnect();
        ak_thread_join(g_sta_conn_thread_id);
        g_sta_conn_thread_id = AK_INVALID_TASK;
    }
    ak_print_normal("Create AP SSID:%s, 11n: %s, key mode:%d, key:%s, channel:%d\n", 
        ap_cfg.ssid, ap_cfg.mode?"enable":"disable", 
        ap_cfg.enc_protocol, ap_cfg.enc_protocol?(char*)ap_cfg.key:"",
        ap_cfg.channel);

    //wifi_init(0);

    //change wifi to ap mode
    wifi_set_mode(WIFI_MODE_AP);
	ak_sleep_ms(10);
    if(wifi_create_ap(&ap_cfg) == 0)
    {
        //init network proto
        wifi_netif_init();
    }
    else
    {
        ak_print_error(" wifi_create_ap fail \n");
    }
}

/*****************************************
 * @brief stop wifi ap
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void wifi_ap_stop(int argc, char **args)
{
    if (argc != 1)
    {
        ak_print_normal("%s",help_ap[1]);
        return;
    }
    ak_print_normal("stop wifi ap...\n");
    wifi_destroy_ap();
}

/*
static void cmd_wifi_sdio(int argc, char **args)
{
	uint32_t reset_flag;

	if (strcmp(args[0], "resetflag") == 0)
	{
		if(argc == 1)
		{
			if(sdio_flag_get(&reset_flag) < 0)
			{
				ak_print_normal("get flag error\n");
			}
			else
			{
				ak_print_normal("resetflag %d\n", reset_flag);
			}
		}
		else if(argc == 2)
		{
			reset_flag = atoi(args[1]);
			sdio_flag_set(reset_flag);
		}
		else
		{
			ak_print_normal("%s",help_sdio[1]);
		}
	}
	else
	{
		ak_print_normal("%s",help_sdio[1]);
	}
	
}
*/
#if 1
static void wifi_p2p_start(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	atbm_wifi_p2p_start();
}

static void wifi_p2p_find(int argc, char **args)
{
	int timeout;
	if (argc != 2)
	{
		ak_print_normal("params error");
		return;
	}
	timeout = atoi(args[1]);
	atbm_wifi_p2p_find(timeout);
}
static void wifi_p2p_find_accept(int argc, char **args)
{
	int go_intent;
	if (argc != 2)
	{
		ak_print_normal("params error");
		return;
	}
	go_intent = atoi(args[1]);
	atbm_wifi_p2p_find_accept(go_intent);
}
static void wifi_p2p_find_stop(int argc, char **args)
{
	int timeout;
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	timeout = atoi(args[1]);
	atbm_wifi_p2p_find_stop(timeout);
}
static void wifi_p2p_show_peers(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	atbm_wifi_p2p_show_peers();
}
static void wifi_p2p_go_start(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	atbm_wifi_p2p_go_start();
}

int hex_to_dec(char dec){
	if(dec >= 'a' && dec <= 'z')
		return (dec - 'a' + 10);
	if(dec >= 'A' && dec <= 'Z')
		return (dec - 'A' + 10);
	if(dec >= '0' && dec <= '9')
		return (dec - '0');
	return 0;
}

static void wifi_p2p_connect(int argc, char **args)
{
	unsigned char mac[6];
	char *pos;
	int go_intent, i = 0, high;
	if (argc != 3)
	{
		ak_print_normal("params error");
		return;
	}

	pos = args[1];
	while(*pos != '\0'){
		if(*pos == ':' || *pos == '-'){
			*pos = '\0';
			high = hex_to_dec(*(pos - 1));
			*(pos - 1) = '\0';
			mac[i] = hex_to_dec(*(pos - 2)) * 16 + high;
			if(i == 4){
				high = hex_to_dec(*(pos + 2));
				*(pos + 2) = '\0';
				mac[5] = hex_to_dec(*(pos + 1)) * 16 + high;
				break;
			}
			i++;
		}
		pos++;
	}
//	sscanf(args[1], "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	ak_print_normal("mac:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	go_intent = atoi(args[2]);
	atbm_wifi_p2p_connect(mac, go_intent);
}

static void wifi_p2p_stop(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	atbm_wifi_p2p_stop();
}

static void wifi_get_rssi(int argc, char **args)
{
	if (argc != 1)
	{
		ak_print_normal("params error");
		return;
	}
	atbm_wifi_p2p_stop();
}

#endif
#if 0
extern void atbm_wifi_wps_control(int argc, char **args);
static void wifi_wps_control(int argc, char **args)
{
	atbm_wifi_wps_control(argc, args);
}
#endif
/* wifi command entry table, we can add new cmd hear */
static wifi_cmd_entry_t s_sta_cmd_table[] = 
{
/* STA cmd start*/
	{"testcontime", wifi_sta_testcontime},
    {"conn",       wifi_sta_conn},
    {"disconn",    wifi_sta_disconn},
    {"status",     wifi_sta_status},
    {"dhcp",       wifi_sta_dhcp},
    {"config",     wifi_info_config},
    {"clearcfg",   wifi_config_clear},
    {"wakeup",     wifi_sta_wakeup},
    {"sleep",      wifi_sta_sleep},
    {"init",       wifi_sta_init},
    {"keepalive",  wifi_sta_keepalive},
    {"scan",       wifi_scan_demo},
    {"log",       wifi_start_log},
    {"sdio",       sdio_dump},
	{"smartconfig", wifi_sta_smartconfig},
	
	{"start_tx",       wifi_StartTx},
	{"stop_tx",       wifi_StopTx},
	{"start_rx",       wifi_StartRx},
	{"stop_rx",       wifi_StopRx},
	{"mfgcarriertone",       wifi_MFGCarrierTone},
#if 1
	{"p2p_start", wifi_p2p_start},
	{"p2p_find", wifi_p2p_find},
	{"p2p_find_accept", wifi_p2p_find_accept},
	{"p2p_find_stop", wifi_p2p_find_stop},
	{"p2p_show_peers", wifi_p2p_show_peers},
	{"p2p_go_start", wifi_p2p_go_start},
	{"p2p_connect", wifi_p2p_connect},
	{"p2p_stop", wifi_p2p_stop},
	{"get_rssi", wifi_get_rssi},
//	{"wps_mode", wifi_wps_control},
#endif
#ifdef WPS_SUPPORT
//	{"wps", 	   wifi_wps_demo},  
#endif
/* add for driver test*/
#if DRIVER_TEST
    {"fastlink",   wifi_sta_fastlink},
    {"hscfg",      wifi_sta_hscfg},
    {"ps",         wifi_sta_ps},
    {"pscfg",      wifi_sta_pscfg},
    {"auto_reconn",wifi_sta_reconnect},
    {"txpower",    wifi_sta_txpower},
#endif
    {"NULL",       NULL}

};

static wifi_cmd_entry_t s_ap_cmd_table[] = 
{
/* ap cmd start*/
    {"start",      wifi_ap_start},
    {"stop",       wifi_ap_stop},
    {"NULL",       NULL}
};

/*****************************************
 * @brief wifi cmd entry check
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return int 0 on success, -1 on faild
 *****************************************/
static int cmd_entry_check(wifi_cmd_entry_t * cmd_table, int argc, char **args)
{
    int i,ret;

    //find the right cmd name
    for(i = 0; cmd_table[i].cmd_entry != NULL; i++)
    {
        if (strcmp(cmd_table[i].cmd_name, args[0]) == 0)
        {
            //find and execute entry function
            cmd_table[i].cmd_entry(argc, args);
            return 0;
        }
    }

    //set_error_no(ERROR_TYPE_INVALID_ARG);
    return -1;
}

/*****************************************
 * @brief wifi config function
 * 测试WIFI连接,
 * 连接到指定名字的路由器，加密模式和频道自动适应
 * 密码长度在WPA或WPA2模式下8 <= len <= 64;在WEP模式下必须为5或13
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void cmd_wifi_sta(int argc, char **args)
{
	if((cfg_handle = ak_ini_init("APPINI")) == NULL){
        ak_print_error_ex("open ini fail\n");
    }
	
    if(cmd_entry_check(s_sta_cmd_table, argc, args) != 0)
    {       
        ak_print_normal(" %s", help_sta[1]);
    }
	ak_ini_flush_data(cfg_handle);	
	ak_ini_destroy(cfg_handle);		
}

/*****************************************
 * @brief wifi config function
 * @param arc[in]  the cmd param number
 * @param arg[in]  the cmd param
 * @return void
 *****************************************/
static void cmd_wifi_ap(int argc, char **args)
{
    if(cmd_entry_check(s_ap_cmd_table, argc, args) != 0)
    {       
        ak_print_normal(" %s", help_ap[1]);
    }
}

/*****************************************
 * @brief register wifi demo command
 * @param [void]  
 * @return 0
 *****************************************/
int wifi_demo_init()
{
	cmd_register("ap", cmd_wifi_ap, help_ap);
	cmd_register("sta", cmd_wifi_sta, help_sta);
	return 0;
}

cmd_module_init(wifi_demo_init)

