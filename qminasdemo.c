#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "qmi_client.h"
#include "network_access_service_v01.h"

#define LOG printf

int setModePref(int mode);
int get_RSSI();
static qmi_idl_service_object_type qminasdemo_uim_svc_obj;
static qmi_client_type             qminasdemo_uim_svc_client;
static qmi_client_os_params        qminasdemo_uim_os_params;
static qmi_client_type             qminasdemo_uim_notifier;

static void qminasdemo_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
  return;
}

int qminasdemo_qmi_init(void)
{
  qmi_client_error_type client_err = QMI_NO_ERR;
  int num_retries = 0;

  qminasdemo_uim_svc_obj = nas_get_service_object_v01();

  do {
       if ( num_retries != 0) {
         sleep(1);
         LOG("qmi_client_init_instance status retry : %d", num_retries);
       }

       client_err = qmi_client_init_instance(qminasdemo_uim_svc_obj,
                                               QMI_CLIENT_INSTANCE_ANY,
                                               qminasdemo_ind_cb,
                                               NULL,
                                               &qminasdemo_uim_os_params,
                                               (int) 4,
                                               &qminasdemo_uim_svc_client);
       num_retries++;
   } while ( (client_err != QMI_NO_ERR) && (num_retries < 2) );


   if ( client_err == QMI_NO_ERR )
   {
     client_err =  qmi_client_notifier_init(qminasdemo_uim_svc_obj,
                                              &qminasdemo_uim_os_params,
                                              &qminasdemo_uim_notifier
                                              );
  }
  return 0;
}

void qminasdemo_get_SignalStrength( void )
{
  nas_get_signal_strength_req_msg_v01 nas_req_msg;
  nas_get_signal_strength_resp_msg_v01 resp;
  qmi_client_error_type rc;
  char *radio_type = NULL;

  memset(&nas_req_msg, 0, sizeof(nas_get_signal_strength_req_msg_v01));
  memset(&resp, 0, sizeof(nas_get_signal_strength_resp_msg_v01)); 

  nas_req_msg.request_mask_valid = 1;
  nas_req_msg.request_mask = 0xFF;

  rc = qmi_client_send_msg_sync(qminasdemo_uim_svc_client,
                            QMI_NAS_GET_SIGNAL_STRENGTH_REQ_MSG_V01, 
                            &nas_req_msg, 
                            sizeof(nas_req_msg), 
                            &resp, 
                            sizeof(resp),
                            10000);

  if (QMI_NO_ERR != rc)
  {
    LOG("qmi get sig err=%d\n",
                  rc);
  }
  else if (QMI_NO_ERR != resp.resp.result)
  {
    LOG("qmi get sig: failed response err=%d\n",
                  resp.resp.error);

  }
  else
  {
    //LOG("ok here\n");
  }

  switch(resp.signal_strength.radio_if){
    case 4:
      radio_type="gsm";
      break;
    case 5:
      radio_type="umts";
      break;
    case 8:
      radio_type="lte";
      break;
    default:
      radio_type="err";
  }

  printf("{\n");
  printf("        \"type\": \"%s\",\n",radio_type); // 0x08 - LTE, 0x05 - UMTS 
  printf("        \"rssi\": \"%d\",\n",resp.signal_strength.sig_strength);
  printf("        \"rsrp\": \"%d\"\n",resp.lte_rsrp);
  printf("        \"rsrq\": \"%d\"\n",resp.rsrq.rsrq); 
  printf("        \"snr\": \"%d\"\n",resp.snr/10);
  printf("}\n");  

  // printf("signal_stregth-sig_strength %d, radio if 0x%x\n", resp.signal_strength.sig_strength, resp.signal_strength.radio_if);
}

void qminasdemo_set_mode_pref(int mode)
{
  nas_set_system_selection_preference_req_msg_v01  nas_req_msg;
  nas_set_system_selection_preference_resp_msg_v01 resp;
  qmi_client_error_type rc;
  char *radio_type = NULL;

  memset(&nas_req_msg, 0, sizeof(nas_set_system_selection_preference_req_msg_v01));
  memset(&resp, 0, sizeof(nas_set_system_selection_preference_resp_msg_v01)); 

  nas_req_msg.mode_pref_valid =  1;
  nas_req_msg.mode_pref = mode;


  rc = qmi_client_send_msg_sync(qminasdemo_uim_svc_client,
                            QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01, 
                            &nas_req_msg, 
                            sizeof(nas_req_msg), 
                            &resp, 
                            sizeof(nas_set_system_selection_preference_resp_msg_v01),
                            10000);
#ifdef LOG_ENABLE
  if (QMI_NO_ERR != rc)
  {
    LOG("qmi set mode network err=%d\n", rc);
  }else{
    LOG("qmi set mode network ok\n", rc);
  }
#endif
}

void qminasdemo_qmi_release(void)
{
  qmi_client_release(qminasdemo_uim_svc_client);
}

int setModePref(int mode){
  qminasdemo_qmi_init();

  qminasdemo_set_mode_pref(mode);
 
  qminasdemo_qmi_release();
}

int get_RSSI()
{
  qminasdemo_qmi_init();

  qminasdemo_get_SignalStrength();
 
  qminasdemo_qmi_release();

  return 1;
}
