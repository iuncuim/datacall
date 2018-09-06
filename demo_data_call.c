/**
  @file
  app_data_call.c

  @brief
  This file provides a demo to inplement a data call in mdm9607 embedded linux
  plane.

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "dsi_netctrl.h"



//#define LOG_ENABLE

#define SASTORAGE_DATA(addr)    (addr).__ss_padding


enum app_tech_e {
  app_tech_min = 0,
  app_tech_umts = app_tech_min,
  app_tech_cdma,
  app_tech_1x,
  app_tech_do,
  app_tech_lte,
  app_tech_auto,
  app_tech_max
};

typedef struct {
  enum app_tech_e tech;
  unsigned int dsi_tech_val;
} app_tech_map_t;

app_tech_map_t tech_map[] = {
  {app_tech_umts, DSI_RADIO_TECH_UMTS},
  {app_tech_cdma, DSI_RADIO_TECH_CDMA},
  {app_tech_1x, DSI_RADIO_TECH_1X},
  {app_tech_do, DSI_RADIO_TECH_DO},
  {app_tech_lte, DSI_RADIO_TECH_LTE},
  {app_tech_auto, DSI_RADIO_TECH_UNKNOWN}
};

enum app_call_status_e
{
  app_call_status_idle,
  app_call_status_connecting,
  app_call_status_connected,
  app_call_status_disconnecting
};

typedef struct {
  dsi_hndl_t handle;
  enum app_tech_e tech;
  int family;
  int profile;
  enum app_call_status_e call_status;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  struct {
    dsi_qos_id_type flow_id;
    dsi_qos_granted_info_type qos_granted_info;
  } qos_info;
} app_call_info_t;


struct event_strings_s
{
  dsi_net_evt_t evt;
  char * str;
};

typedef struct{
	char 			*pAPN;			//String APN
	char 			*pUsername;		//APN user name
	char 			*pPsw;			//APN password
	dsi_auth_pref_t auth;			//Auth pref
}t_arg;

t_arg setting;
static const char *optString = "va:u:p:h?";
static const struct option longOpts[] = {
		{"version", no_argument, NULL,'v'},
		{"apn", required_argument, NULL,'a'},
		{"user", required_argument, NULL,'u'},
		{"psw", required_argument, NULL,'p'},
		{"auth", required_argument, NULL,0},
		{"help", no_argument, NULL,'h'},
		{ NULL, no_argument, NULL, 0 }
};

struct event_strings_s event_string_tbl[DSI_EVT_MAX] =
{
  { DSI_EVT_INVALID, "DSI_EVT_INVALID" },
  { DSI_EVT_NET_IS_CONN, "DSI_EVT_NET_IS_CONN" },
  { DSI_EVT_NET_NO_NET, "DSI_EVT_NET_NO_NET" },
  { DSI_EVT_PHYSLINK_DOWN_STATE, "DSI_EVT_PHYSLINK_DOWN_STATE" },
  { DSI_EVT_PHYSLINK_UP_STATE, "DSI_EVT_PHYSLINK_UP_STATE" },
  { DSI_EVT_NET_RECONFIGURED, "DSI_EVT_NET_RECONFIGURED" },
  { DSI_EVT_QOS_STATUS_IND, "DSI_EVT_QOS_STATUS_IND" },
  { DSI_EVT_NET_NEWADDR, "DSI_EVT_NET_NEWADDR" },
  { DSI_EVT_NET_DELADDR, "DSI_EVT_NET_DELADDR" },
  { DSI_NET_TMGI_ACTIVATED, "DSI_NET_TMGI_ACTIVATED" },
  { DSI_NET_TMGI_DEACTIVATED, "DSI_NET_TMGI_DEACTIVATED" },
  { DSI_NET_TMGI_LIST_CHANGED, "DSI_NET_TMGI_LIST_CHANGED" },
};


/* demo apn */
//static char apn[] = "wonet";

static app_call_info_t app_call_info;

static void app_net_ev_cb( dsi_hndl_t hndl,
                     void * user_data,
                     dsi_net_evt_t evt,
                     dsi_evt_payload_t *payload_ptr )
{
  int i;

  char suffix[256];

#ifdef LOG_ENABLE
  printf("hdl 0x%x evt %d cb\n", hndl, evt);
#endif
  if (evt > DSI_EVT_INVALID && evt < DSI_EVT_MAX)
  {
    app_call_info_t* call = (app_call_info_t*) user_data;
    memset( suffix, 0x0, sizeof(suffix) );

    /* Update call state */
    switch( evt )
    {
      case DSI_EVT_NET_NO_NET:
        call->call_status =  app_call_status_idle;
        break;

      case DSI_EVT_NET_IS_CONN:
        call->call_status = app_call_status_connected;
        break;

      case DSI_EVT_QOS_STATUS_IND:
        break;

      case DSI_EVT_PHYSLINK_DOWN_STATE:
      case DSI_EVT_PHYSLINK_UP_STATE:
      case DSI_EVT_NET_RECONFIGURED:
      case DSI_EVT_NET_NEWADDR:
      case DSI_EVT_NET_DELADDR:
      case DSI_NET_TMGI_ACTIVATED:
      case DSI_NET_TMGI_DEACTIVATED:
      case DSI_NET_TMGI_LIST_CHANGED:
        /* Do nothing */
        break;

      default:
#ifdef LOG_ENABLE
        printf("Received unsupported event [%d]\n", evt);
#endif
        return;
    }

    /* Debug message */
    for (i=0; i<DSI_EVT_MAX; i++)
    {
      if (event_string_tbl[i].evt == evt)
      {
#ifdef LOG_ENABLE
        printf("<<< callback received event [%s]%s\n",
                      event_string_tbl[i].str, suffix);
#endif
        break;
      }
    }
  }
}

static void app_create_data_call(enum app_tech_e tech, int ip_family, int profile)
{
  dsi_call_param_value_t param_info;

  /* obtain data service handle */
  app_call_info.handle = dsi_get_data_srvc_hndl(app_net_ev_cb, (void*) &app_call_info);

#ifdef LOG_ENABLE
  printf("app_call_info.handle 0x%x\n",app_call_info.handle );
#endif
  app_call_info.tech = tech;
  app_call_info.profile = profile;
  app_call_info.call_status = app_call_status_idle;


   /* set data call param - tech pref */
  param_info.buf_val = NULL;
  param_info.num_val = (int)tech_map[tech].dsi_tech_val;
#ifdef LOG_ENABLE
  printf("Setting tech to %d\n", tech_map[tech].dsi_tech_val);
#endif
  dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_TECH_PREF, &param_info);

    
  /* set data call param - other */
  switch (app_call_info.tech)
  {
  case app_tech_umts:
  case app_tech_lte:

    if(setting.pAPN != NULL && strlen(setting.pAPN)){
    	param_info.buf_val = setting.pAPN;
    	param_info.num_val = strlen(setting.pAPN);
#ifdef LOG_ENABLE
    	printf("Setting APN to %s\n", setting.pAPN);
#endif
    	dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_APN_NAME, &param_info);
    }else{
    	param_info.buf_val = NULL;
    	param_info.num_val = 0;
    }
    break;
  default:
    break;
  }

  param_info.buf_val = NULL;
  param_info.num_val = setting.auth;
  dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_AUTH_PREF, &param_info);

  switch (ip_family)
  {
  case DSI_IP_VERSION_4:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
#ifdef LOG_ENABLE
    printf("Setting family to %d\n", ip_family);
#endif
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;

  case DSI_IP_VERSION_6:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
#ifdef LOG_ENABLE
    printf("Setting family to %d\n", ip_family);
#endif
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;

  case DSI_IP_VERSION_4_6:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
#ifdef LOG_ENABLE
    printf("Setting family to %d\n", ip_family);
#endif
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;
  default:
    /* don't set anything for IPv4 */
    break;
  }

  if(setting.pUsername != NULL && strlen(setting.pUsername)){
	  param_info.buf_val = setting.pUsername;
	  param_info.num_val = strlen(setting.pUsername);
#ifdef LOG_ENABLE
	  printf("Setting User Name to %s\n", setting.pUsername);
#endif
	  dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_USERNAME, &param_info);
  }

  if(setting.pPsw != NULL && strlen(setting.pPsw)){
	  param_info.buf_val = setting.pPsw;
	  param_info.num_val = strlen(setting.pPsw);
#ifdef LOG_ENABLE
	  printf("Setting Password  to %s\n", setting.pPsw);
#endif
	  dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_PASSWORD, &param_info);
  }


#define DSI_PROFILE_3GPP2_OFFSET (1000)

  if (profile > 0)
  {
    if( profile > DSI_PROFILE_3GPP2_OFFSET )
    {
      param_info.num_val = (profile - DSI_PROFILE_3GPP2_OFFSET);
#ifdef LOG_ENABLE
      printf("Setting 3GPP2 PROFILE to %d\n", param_info.num_val);
#endif
      dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info);
    }
    else
    {
      param_info.num_val = profile;
#ifdef LOG_ENABLE
      printf("Setting 3GPP PROFILE to %d\n", param_info.num_val);
#endif
      dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info);
    }
  }
}

void display_usage( void )
{
    printf("Options:\n"
    		"  --version, -v:                     Programm version\n"
    		"  --apn <APN>, -a <APN>:             Set APN\n"
    		"  --user <username>, -u <username>:  Set APN user name (if any)\n"
    		"  --psw <password>, -u <password>:   Set APN password (if any)\n"
    		"  --auth <type>:                     Set type authorization (no, pap, chap, both)\n"
    		);
    exit( EXIT_FAILURE );
}

int main(int argc, char * argv[])
{
  int rval;
  char ip_str[20];
  dsi_addr_info_t addr_info;
  int opt = 0;
  int longIndex;
  pid_t pid;


  setting.pAPN = NULL;
  setting.pUsername = NULL;
  setting.pPsw = NULL;
  setting.auth = DSI_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;

  opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  while( opt != -1 ) {
	  switch( opt ) {
	  case 'v':
		  printf("Ver: 0.1\n");
		  break;
	  case 'a':
#ifdef LOG_ENABLE
		  printf("Set APN=%s\n",optarg);
#endif
		  setting.pAPN = optarg;
		  break;
	  case 'u':
#ifdef LOG_ENABLE
		  printf("Set User Name=%s\n",optarg);
#endif
		  setting.pUsername = optarg;
		  break;
	  case 'p':
#ifdef LOG_ENABLE
		  printf("Set Password APN=%s\n",optarg);
#endif
		  setting.pPsw = optarg;
		  break;
	  case 'h':
	  case '?':
		  display_usage();
		  break;
	  case 0:
		  if( strcmp( "auth", longOpts[longIndex].name ) == 0 ) {
			  if(optarg == NULL){
				  printf("Error options --auth\n");
				  exit( EXIT_FAILURE );
			  }
			  if(strcmp( "no", optarg ) == 0){
				  setting.auth = DSI_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;
#ifdef LOG_ENABLE
				  printf("Set auth NONE\n");
#endif
			  }else
			  if(strcmp( "pap", optarg ) == 0){
				  setting.auth = DSI_AUTH_PREF_PAP_ONLY_ALLOWED;
#ifdef LOG_ENABLE
				  printf("Set auth PAP\n");
#endif
			  }else
			  if(strcmp( "chap", optarg ) == 0){
				  setting.auth = DSI_AUTH_PREF_CHAP_ONLY_ALLOWED;
#ifdef LOG_ENABLE
				  printf("Set auth CHAP\n");
#endif
			  }else
			  if(strcmp( "both", optarg ) == 0){
				  setting.auth = DSI_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED;
#ifdef LOG_ENABLE
				  printf("Set auth PAP and CHAP both\n");
#endif
			  }else{
				  printf("Error argument for options --auth\n");
			  }
		  }
		  break;
	  default:
		  printf("Error options\n");
		  break;

	  }
	  opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  }




  memset( &app_call_info, 0x0, sizeof(app_call_info) );

 (void)pthread_mutex_init( &app_call_info.mutex, NULL );
 (void)pthread_cond_init( &app_call_info.cond, NULL );

  if (DSI_SUCCESS !=  dsi_init(DSI_MODE_GENERAL))
  {
    printf("%s","dsi_init failed !!\n");
    return -1;
  }
   
  sleep(2);
  
  app_create_data_call(app_tech_umts, DSI_IP_VERSION_4, 0);
#ifdef LOG_ENABLE
  printf("Doing Net UP\n");
#endif
  
  dsi_start_data_call(app_call_info.handle);
  app_call_info.call_status = app_call_status_connecting;
 
  sleep (3);

  memset(&addr_info, 0x0, sizeof(dsi_addr_info_t));
  rval = dsi_get_ip_addr(app_call_info.handle, &addr_info, 1);

  switch(pid=fork()) {
  case -1:
	  perror("fork");
	  exit(1);
  default:
	  printf("{\n");
	  printf("	\"pdp-type\": \"ipv4\",\n");
	  printf("	\"ip-family\": \"ipv4\",\n");
	  printf("	\"handle\": \"0x%x\",\n",app_call_info.handle);
	  printf("	\"status\": ");
	  switch(app_call_info.call_status){
	  case app_call_status_idle:
		  printf("\"idle\",\n");
		  break;
	  case app_call_status_connecting:
		  printf("\"connecting\",\n");
		  break;
	  case app_call_status_connected:
		  printf("\"connected\",\n");
		  break;
	  case app_call_status_disconnecting:
		  printf("\"disconnecting\",\n");
		  break;
	  default:
		  printf("\"unknown\",\n");
		  break;

	  }
	  printf("	\"ipv4\": {\n");
	  printf("		\"ip\": \"%d.%d.%d.%d\",\n", SASTORAGE_DATA(addr_info.iface_addr_s.addr)[0], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[1], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[2], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[3]);
	  printf("		\"dns1\": \"%d.%d.%d.%d\",\n", SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[0], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[1], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[2], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[3]);
	  printf("		\"dns2\": \"%d.%d.%d.%d\",\n", SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[0], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[1], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[2], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[3]);
	  printf("		\"gateway\": \"%d.%d.%d.%d\",\n", SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[0], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[1], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[2], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[3]);
	  printf("		\"subnet\": \"%d\"\n", addr_info.iface_mask);
	  printf("	}\n");
	  printf("}\n");
	  exit(1);
	  case 0:
		  if (daemon(0,0) != 0)
			  err(EXIT_FAILURE, "Cannot daemonize");
		  do{
			  sleep (10000000);
		  }while(1);
		  break;
  }

}
