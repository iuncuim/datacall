#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



#include "qmi_client.h"
#include "wireless_data_service_v01.h"

#define LOG printf
//#define DEBUG

static qmi_idl_service_object_type qmiuimdemo_wds_svc_obj;
static qmi_client_type             qmiuimdemo_wds_svc_client;
static qmi_client_os_params        qmiuimdemo_wds_os_params;
static qmi_client_type             qmiuimdemo_wds_notifier;


/*
  Callback func.
  When qmi got unsolited message to deliver to ap from modem,
	this function will be invoked.
  NOTE: this func is called in a separate thread from the main thread,
	this thread is created by qmi libraries
*/
static void qmiwdsdemo_ind_cb
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

int qmiwdsdemo_qmi_init(void)
{
  qmi_client_error_type client_err = QMI_NO_ERR;
  int num_retries = 0;

  qmiuimdemo_wds_svc_obj = wds_get_service_object_v01( );

  do {
       if ( num_retries != 0) {
         sleep(1);
         LOG("qmi_client_init_instance status retry : %d", num_retries);
       }

       client_err = qmi_client_init_instance(qmiuimdemo_wds_svc_obj,
                                               QMI_CLIENT_INSTANCE_ANY,
                                               qmiwdsdemo_ind_cb,
                                               NULL,
                                               &qmiuimdemo_wds_os_params,
                                               (int) 4,
                                               &qmiuimdemo_wds_svc_client);
       num_retries++;
   } while ( (client_err != QMI_NO_ERR) && (num_retries < 2) );


   if ( client_err == QMI_NO_ERR )
   {
     client_err =  qmi_client_notifier_init(qmiuimdemo_wds_svc_obj,
                                              &qmiuimdemo_wds_os_params,
                                              &qmiuimdemo_wds_notifier
                                              );
  }
  return 0;
}


void qmiwdsdemo_qmi_release(void)
{
  qmi_client_release(qmiuimdemo_wds_svc_client);
}




int GetStatusConnection(){
	wds_get_last_data_call_status_req_msg_v01 request={0};
	wds_get_last_data_call_status_resp_msg_v01 qmi_response;
	qmi_client_error_type		qmi_err_code = 0;

	qmiwdsdemo_qmi_init();

	memset(&qmi_response, 0, sizeof(wds_get_last_data_call_status_resp_msg_v01));
	qmi_err_code = qmi_client_send_msg_sync(qmiuimdemo_wds_svc_client,
                            QMI_WDS_GET_LAST_DATA_CALL_STATUS_REQ_V01,
                            NULL,//&request,
                            0,//sizeof(request),
                            &qmi_response,
                            sizeof(qmi_response),
                            10000);
	if (qmi_err_code != QMI_NO_ERR){
#ifdef DEBUG
		LOG("qmi status connection err=%d\n",qmi_err_code);
#endif //#ifdef DEBUG
		qmiwdsdemo_qmi_release();
		return 0;
	}

	if(qmi_response.resp.result != QMI_NO_ERR ){
#ifdef DEBUG
		LOG("qmi get status connection err=%d\n",qmi_response.resp.error);
#endif //#ifdef DEBUG
		qmiwdsdemo_qmi_release();
		return 0;
	}

	if(qmi_response.data_call_type_valid && qmi_response.data_call_status == WDS_DATA_CALL_ACTIVATED_V01){
#ifdef DEBUG
		LOG("Connection is activated\n");
#endif //#ifdef DEBUG
		qmiwdsdemo_qmi_release();
		return 1;
	}else{
#ifdef DEBUG
		LOG("Data call type valid=%d\n", qmi_response.data_call_type_valid);
		LOG("Data call status=%d\n", qmi_response.data_call_status);
#endif //#ifdef DEBUG
		qmiwdsdemo_qmi_release();
		return 0;
	}

}
















