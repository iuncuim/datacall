#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "qmi_client.h"
#include "user_identity_module_v01.h"

#define LOG printf

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

  if ((raw_iccid_ptr == NULL) || (raw_iccid_len == NULL))
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
  
  uint8_t gw_path[] = {0x3F, 0x00, 0x7F, 0x20, 0x00}; //{0x3F, 0x00, 0x7F, 0xFF, 0x00};
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
    LOG("qmi read transparent: failed response err=%d\n",
                  resp.resp.error);

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
    LOG("qmi read transparent: failed response err=%d\n",
                  resp.resp.error);

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
  printf("\"%s\"\n", imsi);
  if(imsi != NULL)
    free(imsi);
   
  qmiuimdemo_qmi_release();

  return 1;
}

int get_ICCID(int argc , char ** argv)
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
  }

  
   if(iccid != NULL)
    free(iccid);
   
  qmiuimdemo_qmi_release();

  return 1;
}
