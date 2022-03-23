#include "atbm_hal.h"

#if CONFIG_P2P

int inited = 0;

int wifi_init(){
	if(!inited){
		atbm_wifi_hw_init();
		inited = 1;
	}else{
		wifi_printk(WIFI_ALWAYS, "Wifi inited already!!\n");
	}
}

int wifi_p2p_start(){
	return atbm_wifi_p2p_start();
}

int wifi_p2p_find(int go_intent){
	return atbm_wifi_p2p_find_accept(go_intent);
}

int p2p_find_stop(int timeout){
	return atbm_wifi_p2p_find_stop(timeout);
}

int p2p_show_peers(){
	return atbm_wifi_p2p_show_peers();
}

int wifi_p2p_conn(char *mac, int go_intent){
	atbm_uint8 peer_mac[6];
	int i;

	iot_printf("mac: %s\n", mac);
	ATBM_ASSERT(strlen(mac) >= 17);

	for(i = 0; i < 6; i++){
		peer_mac[i] = hex2byte(&mac[3 * i]);
	}
	iot_printf("mac: "MACSTR"\n", MAC2STR(peer_mac));
	return atbm_wifi_p2p_connect(peer_mac, go_intent);
}

int p2p_go_start(){
	return atbm_wifi_p2p_go_start();
}

int wifi_p2p_stop(){
	return atbm_wifi_p2p_stop();
}

int p2p_go_stop(){
	return atbm_wifi_p2p_go_stop();
}

#if (PLATFORM == FH8852_RTT)
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(wifi_init, wifi_init(void));
FINSH_FUNCTION_EXPORT(wifi_p2p_start, wifi_p2p_start(void));
FINSH_FUNCTION_EXPORT(wifi_p2p_find, wifi_p2p_find(int go_intent));
FINSH_FUNCTION_EXPORT(p2p_find_stop, p2p_find_stop(int timeout));
FINSH_FUNCTION_EXPORT(p2p_show_peers, p2p_show_peers(void));
FINSH_FUNCTION_EXPORT(wifi_p2p_conn, wifi_p2p_conn(char *mac, int go_intent));
FINSH_FUNCTION_EXPORT(wifi_p2p_stop, wifi_p2p_stop(void));
FINSH_FUNCTION_EXPORT(p2p_go_start, p2p_go_start(void));
FINSH_FUNCTION_EXPORT(p2p_go_stop, p2p_go_stop(void));
#endif
#endif
#endif

