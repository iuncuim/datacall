#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "qmi_client.h"
#include "user_identity_module_v01.h"


#define LOG printf
//#define DEBUG

int get_IMSI();
int get_ICCID();
static qmi_idl_service_object_type qmiuimdemo_uim_svc_obj;
static qmi_client_type             qmiuimdemo_uim_svc_client;
static qmi_client_os_params        qmiuimdemo_uim_os_params;
static qmi_client_type             qmiuimdemo_uim_notifier;



static char qmiuimdemo_bin_to_hexchar
(
  uint8_t ch
)
{
  if (ch < 0x0a)
  {
    return (ch + '0');
  }
  return (ch + 'a' - 10);
}
  
static void qmiuimdemo_flip_data
(
  uint8_t                      * des,
  const uint8_t                * src,
  uint32_t                       data_len
)
{
  unsigned int     i             =   0;
  unsigned short   temp_path     =   0;

  for(i = 0; i < (data_len + 1) && i <= 2 ; i += 2)
  {
    temp_path = (*(src + i) << 8) |
                (*(src + i + 1));

    memcpy(des, (unsigned char*)&temp_path, sizeof(unsigned short));
    des += sizeof(unsigned short);
  }
} /* qcril_uim_qmi_flip_data */

char * qmiuimdemo_parse_gw_imsi
(
  const unsigned char * raw_imsi_ptr,
  unsigned short        raw_imsi_len,
  int                 * parsed_imsi_len_ptr
)
{
  int             src             = 0;
  int             dst             = 0;
  char          * parsed_imsi_ptr = NULL;

  /* Sanity check on input parameters */
  if ((raw_imsi_ptr == NULL) || (parsed_imsi_len_ptr == NULL))
  {
    return NULL;
  }

  /* Check for the length of IMSI bytes in the first byte */
  *parsed_imsi_len_ptr = *raw_imsi_ptr;
  if (*parsed_imsi_len_ptr >= raw_imsi_len)
  {
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Allocate required amount of memory for IMSI in ASCII string format,
     note that it is freed by the caller */
  parsed_imsi_ptr = malloc((2 * (*parsed_imsi_len_ptr)));
  if (parsed_imsi_ptr == NULL)
  {
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Compose IMSI */
  memset(parsed_imsi_ptr, 0, (2 * (*parsed_imsi_len_ptr)));
  for (src = 1, dst = 0;
      (src <= (*parsed_imsi_len_ptr)) && (dst < ((*parsed_imsi_len_ptr) * 2));
       src++)
  {
    /* Only process lower part of byte for second and subsequent bytes */
    if (src > 1)
    {
      parsed_imsi_ptr[dst] = qmiuimdemo_bin_to_hexchar(raw_imsi_ptr[src] & 0x0F);
      dst++;
    }
    /* Process upper part of byte for all bytes */
    parsed_imsi_ptr[dst] = qmiuimdemo_bin_to_hexchar(raw_imsi_ptr[src] >> 4);
    dst++;
  }

  /* Update IMSI length in bytes - parsed IMSI in ASCII is raw times 2 */
  *parsed_imsi_len_ptr *= 2;

  return parsed_imsi_ptr;
}

char * qmiuimdemo_parse_1x_imsi
(
  const unsigned char * raw_imsi_ptr,
  unsigned short        raw_imsi_len,
  int                 * parsed_imsi_len_ptr
)
{
  uint8_t     i             = 0;
  uint16_t    mcc           = 0;
  uint8_t     mnc           = 0;
  uint32_t    min1          = 0;
  uint16_t    min2          = 0;
  uint16_t    second_three  = 0;
  uint8_t     thousands     = 0xFF;
  uint16_t    last_three    = 0;
  uint8_t     min_to_num[]  = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
  uint8_t     bcd_to_num[]  = { 0xFF, '1', '2', '3', '4', '5', '6', '7', '8',
                              '9', '0', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  char          * parsed_imsi_ptr = NULL;

  /* Sanity check on input parameters */
  if ((raw_imsi_ptr == NULL) || (parsed_imsi_len_ptr == NULL))
  {
    return NULL;
  }

  /* Check for the minumim length of IMSI_M expected */
  if (raw_imsi_len < 10)
  {
    return NULL;
  }

  /* Sanity check for IMSI_M_PROGRAMMED indicator */
  if ((raw_imsi_ptr[7] & 0x80) == 0)
  {
    return NULL;
  }

  /* Update parsed length - null terminated ASCII string length */
  *parsed_imsi_len_ptr = 16;

  /* Allocate the number of bytes */
  parsed_imsi_ptr = malloc(*parsed_imsi_len_ptr);
  if (parsed_imsi_ptr == NULL)
  {
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  memset(parsed_imsi_ptr, 0, *parsed_imsi_len_ptr);

  /* Parse certain parameters */
  mcc           = (((raw_imsi_ptr[9] & 0x03) << 8) + raw_imsi_ptr[8]);
  mnc           = raw_imsi_ptr[6] & 0x7F;
  min1          = ((raw_imsi_ptr[5] <<16) +(raw_imsi_ptr[4] <<8) + raw_imsi_ptr[3]);
  min2          = ((raw_imsi_ptr[2] <<8)  + raw_imsi_ptr[1]);
  /* left 10 bits */
  second_three  = (min1 & 0x00FFC000) >> 14;
  /* middle 4 bits */
  thousands     = (min1 & 0x00003C00) >> 10;
  /* right 10 bits */
  last_three    = (min1 & 0x000003FF);
  thousands     = bcd_to_num[thousands];

  if ((mcc > 999) || (mnc > 99) || (min2 > 999) || (thousands == 0xFF) ||
      (second_three > 999) || (last_three > 999))
  {
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Construct ASCII IMSI_M, format:
     <3_digit_MCC><2_digit_11_12_digits><LS_10_digits_IMSI> */
  /* Update MCC - 3 digits */
  parsed_imsi_ptr[i++] = min_to_num[mcc/100];
  mcc %= 100;
  parsed_imsi_ptr[i++] = min_to_num[mcc/10];
  parsed_imsi_ptr[i++] = min_to_num[mcc%10];
  /* Update MNC - 2 digits */
  parsed_imsi_ptr[i++] = min_to_num[mnc/10];
  parsed_imsi_ptr[i++] = min_to_num[mnc%10];
  /* Update the first 3 digits of IMSI */
  parsed_imsi_ptr[i++] = min_to_num[min2/100];
  min2 %= 100;
  parsed_imsi_ptr[i++] = min_to_num[min2/10];
  parsed_imsi_ptr[i++] = min_to_num[min2%10];
  /* Update the last 7 digits of IMSI */
  parsed_imsi_ptr[i++] = min_to_num[second_three/100];
  second_three %= 100;
  parsed_imsi_ptr[i++] = min_to_num[second_three/10];
  parsed_imsi_ptr[i++] = min_to_num[second_three%10];
  parsed_imsi_ptr[i++] = thousands;
  parsed_imsi_ptr[i++] = min_to_num[last_three/100];
  last_three %= 100;
  parsed_imsi_ptr[i++] = min_to_num[last_three/10];
  parsed_imsi_ptr[i++] = min_to_num[last_three%10];

  return parsed_imsi_ptr;
} /* qcril_uim_parse_1x_imsi */

char * qmiuimdemo_parse_iccid
(
  const unsigned char * raw_iccid_ptr,
  unsigned short        raw_iccid_len,
  int                 * parsed_iccid_len_ptr
)
{
  int src = 0;
  int dst = 0;
  char * parsed_iccid = NULL;

  if ((raw_iccid_ptr == NULL) || !raw_iccid_len)
  {
    return NULL;
  }
  
  parsed_iccid = malloc(2*raw_iccid_len +1 );
  if(NULL == parsed_iccid)
  {
    *parsed_iccid_len_ptr = 0;
    return NULL;
  }
  memset(parsed_iccid, 0, 2*raw_iccid_len +1 );
  
  for(src = 0, dst = 0; (src < raw_iccid_len) && (dst < raw_iccid_len*2); src++)
  {
    parsed_iccid[dst] = qmiuimdemo_bin_to_hexchar(raw_iccid_ptr[src] & 0x0F);
    parsed_iccid[dst+1] = qmiuimdemo_bin_to_hexchar(raw_iccid_ptr[src] >> 4 );
    dst+=2;
  }

  *parsed_iccid_len_ptr = raw_iccid_len*2;

  return parsed_iccid;
}

/*
  Callback func.
  When qmi got unsolited message to deliver to ap from modem,
	this function will be invoked.
  NOTE: this func is called in a separate thread from the main thread,
	this thread is created by qmi libraries
*/
static void qmiuimdemo_ind_cb
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

int qmiuimdemo_qmi_init(void)
{
  qmi_client_error_type client_err = QMI_NO_ERR;
  int num_retries = 0;

  qmiuimdemo_uim_svc_obj = uim_get_service_object_v01();

  do {
       if ( num_retries != 0) {
         sleep(1);
         LOG("qmi_client_init_instance status retry : %d", num_retries);
       }

       client_err = qmi_client_init_instance(qmiuimdemo_uim_svc_obj,
                                               QMI_CLIENT_INSTANCE_ANY,
                                               qmiuimdemo_ind_cb,
                                               NULL,
                                               &qmiuimdemo_uim_os_params,
                                               (int) 4,
                                               &qmiuimdemo_uim_svc_client);
       num_retries++;
   } while ( (client_err != QMI_NO_ERR) && (num_retries < 2) );


   if ( client_err == QMI_NO_ERR )
   {
     client_err =  qmi_client_notifier_init(qmiuimdemo_uim_svc_obj,
                                              &qmiuimdemo_uim_os_params,
                                              &qmiuimdemo_uim_notifier
                                              );
  }
  return 0;
}




char * qmiuimdemo_get_imsi(void )
{
  uim_read_transparent_req_msg_v01 read_params;
  uim_read_transparent_resp_msg_v01 resp;
  qmi_client_error_type rc;
  int imsi_len = 0;
  
  uint8_t gw_path[] = {0x3F, 0x00, 0x7F, 0xFF, 0x00}; //{0x3F, 0x00, 0x7F, 0x20, 0x00}; //{0x3F, 0x00, 0x7F, 0xFF, 0x00};
  uint8_t cdma_path[] = {0x3F, 0x00, 0x7F, 0x25, 0x00};
    
  memset(&read_params, 0, sizeof(uim_read_transparent_req_msg_v01)); 
  memset(&resp, 0, sizeof(uim_read_transparent_resp_msg_v01)); 

  read_params.session_information.session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  read_params.session_information.aid_len = 0;
  read_params.file_id.file_id = 0x6F07;
  read_params.file_id.path_len = 4;
  qmiuimdemo_flip_data(read_params.file_id.path,
                          (uint8_t*)gw_path,
                          read_params.file_id.path_len);

  rc = qmi_client_send_msg_sync(qmiuimdemo_uim_svc_client,
                            QMI_UIM_READ_TRANSPARENT_REQ_V01, 
                            &read_params, 
                            sizeof(read_params), 
                            &resp, 
                            sizeof(resp),
                            10000);

  if (QMI_NO_ERR != rc)
  {
    LOG("qmi read transparent err=%d\n",
                  rc);
  }
  else if (QMI_NO_ERR != resp.resp.result)
  {
    #ifdef DEBUG
    LOG("qmi read transparent: failed response err=%d\n",
                  resp.resp.error);
    #endif //DEBUG
  }
  else
  {
    //LOG("ok here\n");
  }

  return qmiuimdemo_parse_gw_imsi(
                     resp.read_result.content,
                     resp.read_result.content_len,
                     &imsi_len);
}

char * qmiuimdemo_get_iccid(void )
{
  uim_read_transparent_req_msg_v01 read_params;
  uim_read_transparent_resp_msg_v01 resp;
  qmi_client_error_type rc; 
  int iccid_len = 0;
  char *iccid = NULL;
  uint8_t path[] = {0x3F, 0x00, 0x00}; 
  
  memset(&read_params, 0, sizeof(uim_read_transparent_req_msg_v01)); 
  memset(&resp, 0, sizeof(uim_read_transparent_resp_msg_v01));

  read_params.session_information.session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  read_params.session_information.aid_len = 0;
  read_params.file_id.file_id = 0x2FE2;
  read_params.file_id.path_len = 2;
  qmiuimdemo_flip_data(read_params.file_id.path,
                          (uint8_t*)path,
                          read_params.file_id.path_len);

  rc = qmi_client_send_msg_sync(qmiuimdemo_uim_svc_client,
                            QMI_UIM_READ_TRANSPARENT_REQ_V01, 
                            &read_params, 
                            sizeof(read_params), 
                            &resp, 
                            sizeof(resp),
                            10000);

  if (QMI_NO_ERR != rc)
  {
    LOG("qmi read transparent err=%d\n",
                  rc);
  }
  else if (QMI_NO_ERR != resp.resp.result)
  {
    #ifdef DEBUG
    LOG("qmi read transparent: failed response err=%d\n",
                  resp.resp.error);
    #endif //DEBUG
  }
  else
  {
    //LOG("ok here\n");
  }

  return qmiuimdemo_parse_iccid(resp.read_result.content,
                                    resp.read_result.content_len,
                                    &iccid_len);
}

void qmiuimdemo_qmi_release(void)
{
  qmi_client_release(qmiuimdemo_uim_svc_client);
}



int get_IMSI(int argc , char ** argv)
{
  char * imsi = NULL;
  
  qmiuimdemo_qmi_init();

  imsi = qmiuimdemo_get_imsi();
  if(imsi != NULL)
  {
    printf("\"%s\"\n", imsi);  
    free(imsi);
  } else
    printf("\"err\"\n"); 
   
  qmiuimdemo_qmi_release();

  return 1;
}

int get_ICCID()
{
  char * iccid = NULL;
  int i;
  qmiuimdemo_qmi_init();
  
  iccid = qmiuimdemo_get_iccid();
  if(iccid != NULL)
  {
    for (i=0; i < strlen(iccid); i++)
    {
      if (iccid[i] < 0x30 || iccid[i] > 0x39)
      {
        iccid[i]=0;
        break;
      }
    }
    printf("\"%s\"\n", iccid);
  } else
    printf("\"err\"\n"); 
  
   if(iccid != NULL)
    free(iccid);
   
  qmiuimdemo_qmi_release();

  return 1;
}

int getPinState(){
	uim_get_card_status_resp_msg_v01	qmi_response;
	qmi_client_error_type				qmi_err_code = 0;
	unsigned int						i;
	unsigned int						j;
	unsigned int						card_found = 0;

	qmiuimdemo_qmi_init();
	memset(&qmi_response, 0,	sizeof(uim_get_card_status_resp_msg_v01));
	qmi_err_code = qmi_client_send_msg_sync(qmiuimdemo_uim_svc_client,
						QMI_UIM_GET_CARD_STATUS_REQ_V01,
						NULL,
						0,
						&qmi_response,
						sizeof(qmi_response),
						10000);

	if (qmi_err_code != QMI_NO_ERR){
#ifdef DEBUG
		LOG("qmi read card status err=%d\n",qmi_err_code);
#endif //DEBUG
		printf("UIM uninitialized");
		return card_found;
	}
	if(!qmi_response.card_status_valid || qmi_response.resp.result != QMI_RESULT_SUCCESS_V01){
#ifdef DEBUG
		LOG("card_status is not valid !\n");
#endif //DEBUG
		printf("UIM uninitialized");
		return card_found;
	}
	for(i=0; i < QMI_UIM_CARDS_MAX_V01 && i < qmi_response.card_status.card_info_len; i++){

		/**<   Indicates the state of the card. Valid values:\n
			- 0 -- Absent\n
			- 1 -- Present\n
			- 2 -- Error
	    */
#ifdef DEBUG
		LOG("card_info[i].card_state: 0x%x\n",qmi_response.card_status.card_info[i].card_state);
#endif //DEBUG


		 /**<   Indicates the reason for the card error, and is valid only when the card
		 state is Error. Valid values:\n
			  - UIM_CARD_ERROR_CODE_UNKNOWN (0x00) --  Unknown\n
			  - UIM_CARD_ERROR_CODE_POWER_DOWN (0x01) --  Power down\n
			  - UIM_CARD_ERROR_CODE_POLL_ERROR (0x02) --  Poll error\n
			  - UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED (0x03) --  No ATR received\n
			  - UIM_CARD_ERROR_CODE_VOLT_MISMATCH (0x04) --  Volt mismatch\n
			  - UIM_CARD_ERROR_CODE_PARITY_ERROR (0x05) --  Parity error\n
			  - UIM_CARD_ERROR_CODE_POSSIBLY_REMOVED (0x06) --  Unknown, possibly removed\n
			  - UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS (0x07) --  Card returned technical problems\n
			  - UIM_CARD_ERROR_CODE_NULL_BYTES (0x08) --  Card returned NULL bytes\n
			  - UIM_CARD_ERROR_CODE_SAP_CONNECTED (0x09) --  Terminal in SAP mode
		 \n
		 Other values are possible and reserved for future use. When an
		 unknown value is received, it is to be handled as ``Unknown''.
		 */
#ifdef DEBUG
		LOG("card_info[i].error_code: 0x%x\n",qmi_response.card_status.card_info[i].error_code);
#endif //DEBUG

		if (qmi_response.card_status.card_info[i].card_state ==	UIM_CARD_STATE_PRESENT_V01) {
			for (j = 0 ; j < QMI_UIM_APPS_MAX_V01 ; j++) {
				/**<   Indicates the type of the application. Valid values:\n
        				- 0 -- Unknown\n
        				- 1 -- SIM card\n
        				- 2 -- USIM application\n
        				- 3 -- RUIM card\n
        				- 4 -- CSIM application\n
        				- 5 -- ISIM application\n
       	   	   	   	   	   Other values are reserved for the future and are to be handled as
       	   	   	   	   ``Unknown''.
				 */
#ifdef DEBUG
				LOG("Type application=%d\n",qmi_response.card_status.card_info[i].app_info[j].app_type);
#endif //DEBUG

				/**<   Indicates the state of the application. Valid values:\n
        			- 0 -- Unknown\n
        			- 1 -- Detected\n
        			- 2 -- PIN1 or UPIN is required\n
        			- 3 -- PUK1 or PUK for UPIN is required\n
        			- 4 -- Personalization state must be checked\n
        			- 5 -- PIN1 is blocked\n
        			- 6 -- Illegal\n
        			- 7 -- Ready
				*/
#ifdef DEBUG
				LOG("State of application=%d\n",qmi_response.card_status.card_info[i].app_info[j].app_state);
#endif //DEBUG

				if (((qmi_response.card_status.card_info[i].app_info[j].app_type == 1) ||
				(qmi_response.card_status.card_info[i].app_info[j].app_type == 2)) &&
				(qmi_response.card_status.card_info[i].app_info[j].app_state ==	UIM_APP_STATE_READY_V01)) {
#ifdef DEBUG
					LOG("card READY\n");
					LOG("card_info[i].app_type : 0x%x\n", qmi_response.card_status.card_info[i].app_info[j].app_type);
					LOG("card_info[i].app_state : 0x%x\n",qmi_response.card_status.card_info[i].app_info[j].app_state);
#endif //DEBUG
					card_found = 1;
					break;
				}else
				if((qmi_response.card_status.card_info[i].app_info[j].app_type == 2) &&
					(qmi_response.card_status.card_info[i].app_info[j].app_state ==	UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01)){

#ifdef DEBUG
					LOG("card need verified\n");
					LOG("card_info[i].app_type : 0x%x\n", qmi_response.card_status.card_info[i].app_info[j].app_type);
					LOG("card_info[i].app_state : 0x%x\n",qmi_response.card_status.card_info[i].app_info[j].app_state);
#endif //DEBUG
					card_found = 2;
					break;
				}
			}
		}
		if (card_found) {
#ifdef DEBUG
			LOG("card found\n");
#endif //DEBUG
			break;
		}
	}
	qmiuimdemo_qmi_release();
	return card_found;
}

int VerifyPIN(char *pin){
	uim_verify_pin_req_msg_v01 	request={0};
	uim_verify_pin_resp_msg_v01	qmi_response;
	qmi_client_error_type		qmi_err_code = 0;


	if (pin == NULL || strlen(pin) > QMI_UIM_PIN_MAX_V01) {
		printf("Error: Invalid pin\n");
		return -1;
	}
	qmiuimdemo_qmi_init();
	request.session_information.session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
	request.session_information.aid_len = 0;
	request.verify_pin.pin_id = 1;
	/**<   Indicates the PIN ID to be verified. Valid values:\n
        - 1 - PIN1 (also called PIN)\n
        - 2 - PIN2\n
        - 3 - Universal PIN\n
        - 4 - Hidden key
  */
	memcpy( request.verify_pin.pin_value, pin, strlen(pin));
	request.verify_pin.pin_value_len = strlen(pin);
	memset(&qmi_response, 0,	sizeof(uim_verify_pin_resp_msg_v01));
	qmi_err_code = qmi_client_send_msg_sync(qmiuimdemo_uim_svc_client,
                            QMI_UIM_VERIFY_PIN_REQ_V01,
                            &request,
                            sizeof(request),
                            &qmi_response,
                            sizeof(qmi_response),
                            10000);
	if (qmi_err_code != QMI_NO_ERR){
#ifdef DEBUG
		LOG("qmi verify PIN err=%d\n",qmi_err_code);
#endif //DEBUG
		qmiuimdemo_qmi_release();
		return -1;
	}
	if(qmi_response.resp.result != QMI_NO_ERR ){
#ifdef DEBUG
		LOG("qmi request verify PIN err=%d\n",qmi_response.resp.error);
#endif //DEBUG
		qmiuimdemo_qmi_release();
		return -1;
	}

	qmiuimdemo_qmi_release();
	return 0;
}











