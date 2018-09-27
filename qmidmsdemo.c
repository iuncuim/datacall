#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



#include "qmi_client.h"
#include "device_management_service_v01.h"
#include "network_access_service_v01.h"

#define LOG printf
//#define DEBUG

static qmi_idl_service_object_type qmiuimdemo_dms_svc_obj=NULL;
static qmi_client_type             qmiuimdemo_dms_svc_client;
static qmi_client_os_params        qmiuimdemo_dms_os_params;
static qmi_client_type             qmiuimdemo_dms_notifier;


/*
  Callback func.
  When qmi got unsolited message to deliver to ap from modem,
	this function will be invoked.
  NOTE: this func is called in a separate thread from the main thread,
	this thread is created by qmi libraries
*/
static void qmidmsdemo_ind_cb
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


int qmidmsdemo_qmi_init(void)
{
  qmi_client_error_type client_err = QMI_NO_ERR;
  int num_retries = 0;

  num_retries = qmi_init(NULL,NULL);
#ifdef DEBUG
  LOG("qmi_init=%d\n",num_retries);
#endif //DEBUG
  num_retries = 0;

  qmiuimdemo_dms_svc_obj = dms_get_service_object_v01();
#ifdef DEBUG
  LOG("qmiuimdemo_dms_svc_obj=%d\n",qmiuimdemo_dms_svc_obj);
#endif //DEBUG


  do {
       if ( num_retries != 0) {
         sleep(1);
#ifdef DEBUG
         LOG("qmi_client_init_instance status retry : %d\n", num_retries);
#endif //DEBUG
       }

       memset(&qmiuimdemo_dms_os_params, 0, sizeof(qmi_client_os_params));
       memset(&qmiuimdemo_dms_svc_client, 0, sizeof(qmi_client_type));
       client_err = qmi_client_init_instance(qmiuimdemo_dms_svc_obj,
                                               QMI_CLIENT_INSTANCE_ANY,
                                               qmidmsdemo_ind_cb,
                                               NULL,
                                               &qmiuimdemo_dms_os_params,
                                               (int) 10,
                                               &qmiuimdemo_dms_svc_client);
       num_retries++;
#ifdef DEBUG
       if(client_err != QMI_NO_ERR){
    	   LOG("Client init err=%d\n",client_err);
       }
#endif //DEBUG
   } while ( (client_err != QMI_NO_ERR) && (num_retries < 3) );


   if ( client_err == QMI_NO_ERR )
   {
     client_err =  qmi_client_notifier_init(qmiuimdemo_dms_svc_obj,
                                              &qmiuimdemo_dms_os_params,
                                              &qmiuimdemo_dms_notifier
                                              );
  }
  return 0;
}


void qmidmsdemo_qmi_release(void)
{
  qmi_client_release(qmiuimdemo_dms_svc_client);
}

int GetIMEI(){
	dms_get_device_serial_numbers_resp_msg_v01 qmi_response;
	qmi_client_error_type		qmi_err_code = 0;

	qmidmsdemo_qmi_init();

	memset(&qmi_response, 0, sizeof(dms_get_device_serial_numbers_resp_msg_v01));
	qmi_err_code = qmi_client_send_msg_sync(qmiuimdemo_dms_svc_client,
                            QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                            NULL,
                            0,
                            &qmi_response,
                            sizeof(qmi_response),
                            10000);
	if (qmi_err_code != QMI_NO_ERR){
#ifdef DEBUG
		LOG("qmi get IMEI err=%d\n",qmi_err_code);
#endif //#ifdef DEBUG
		qmidmsdemo_qmi_release();
		return -1;
	}

	if(qmi_response.resp.result != QMI_NO_ERR ){
#ifdef DEBUG
		LOG("qmi get IMEI response err=%d\n",qmi_response.resp.error);
#endif //#ifdef DEBUG
		qmidmsdemo_qmi_release();
		return -1;
	}

	if(qmi_response.imei_valid){
#ifdef DEBUG
		LOG("IMEI =%s\n",qmi_response.imei);
#endif //#ifdef DEBUG
		printf("\"%s\"\n",qmi_response.imei);
		qmidmsdemo_qmi_release();
		return 0;
	}else{
#ifdef DEBUG
		LOG("No valid IMEI\n");
#endif //#ifdef DEBUG
		qmidmsdemo_qmi_release();
		return -1;
	}

}





