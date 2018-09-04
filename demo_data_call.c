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

#include "dsi_netctrl.h"

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
static char apn[] = "internet";

static app_call_info_t app_call_info;

static void app_net_ev_cb( dsi_hndl_t hndl,
                     void * user_data,
                     dsi_net_evt_t evt,
                     dsi_evt_payload_t *payload_ptr )
{
  int i;

  char suffix[256];

  printf("hdl 0x%x evt %d cb\n", hndl, evt);
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
        printf("Received unsupported event [%d]\n", evt);
        return;
    }

    /* Debug message */
    for (i=0; i<DSI_EVT_MAX; i++)
    {
      if (event_string_tbl[i].evt == evt)
      {
        printf("<<< callback received event [%s]%s\n",
                      event_string_tbl[i].str, suffix);
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


  printf("app_call_info.handle 0x%x\n",app_call_info.handle );
  app_call_info.tech = tech;
  app_call_info.profile = profile;
  app_call_info.call_status = app_call_status_idle;


   /* set data call param - tech pref */
  param_info.buf_val = NULL;
  param_info.num_val = (int)tech_map[tech].dsi_tech_val;
  printf("Setting tech to %d\n", tech_map[tech].dsi_tech_val);
  dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_TECH_PREF, &param_info);
    
  /* set data call param - other */
  switch (app_call_info.tech)
  {
  case app_tech_umts:
  case app_tech_lte:

    param_info.buf_val = apn;
    param_info.num_val = strlen(apn);
    printf("Setting APN to %s\n", apn);
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_APN_NAME, &param_info);
    break;

  case app_tech_cdma:
    param_info.buf_val = NULL;
    param_info.num_val = DSI_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED;
    printf("%s\n","Setting auth pref to both allowed");
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_AUTH_PREF, &param_info);
    break;

  default:
    break;
  }

  switch (ip_family)
  {
  case DSI_IP_VERSION_4:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
    printf("Setting family to %d\n", ip_family);
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;

  case DSI_IP_VERSION_6:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
    printf("Setting family to %d\n", ip_family);
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;

  case DSI_IP_VERSION_4_6:
    app_call_info.family = ip_family;
    param_info.buf_val = NULL;
    param_info.num_val = ip_family;
    printf("Setting family to %d\n", ip_family);
    dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_IP_VERSION, &param_info);
    break;

  default:
    /* don't set anything for IPv4 */
    break;
  }

#define DSI_PROFILE_3GPP2_OFFSET (1000)

  if (profile > 0)
  {
    if( profile > DSI_PROFILE_3GPP2_OFFSET )
    {
      param_info.num_val = (profile - DSI_PROFILE_3GPP2_OFFSET);
      printf("Setting 3GPP2 PROFILE to %d\n", param_info.num_val);
      dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info);
    }
    else
    {
      param_info.num_val = profile;
      printf("Setting 3GPP PROFILE to %d\n", param_info.num_val);
      dsi_set_data_call_param(app_call_info.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info);
    }
  }
}

int main(int argc, char * argv[])
{
  int rval;
  char ip_str[20];
  dsi_addr_info_t addr_info;

  printf("Ver: 0.2\n");

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
  
  printf("Doing Net UP\n");
  
  dsi_start_data_call(app_call_info.handle);
  app_call_info.call_status = app_call_status_connecting;
 
  sleep (3);

  memset(&addr_info, 0x0, sizeof(dsi_addr_info_t));
  rval = dsi_get_ip_addr(app_call_info.handle, &addr_info, 1);
  printf("IP SUC: %d\n", rval);
  printf("Interface IP address: %d.%d.%d.%d\n", SASTORAGE_DATA(addr_info.iface_addr_s.addr)[0], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[1], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[2], SASTORAGE_DATA(addr_info.iface_addr_s.addr)[3]);
  printf("Gateway IP address: %d.%d.%d.%d\n", SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[0], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[1], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[2], SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[3]);
  printf("Primary DNS address: %d.%d.%d.%d\n", SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[0], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[1], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[2], SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[3]);
  printf("Secondary DNS address: %d.%d.%d.%d\n", SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[0], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[1], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[2], SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[3]);


	do{
		sleep (10000000);
	}while(1);

}
