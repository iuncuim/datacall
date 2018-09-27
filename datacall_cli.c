#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "qmiuimdemo.h"


#define VERSION		"Ver: 0.1\n"

 
static const char *optString = "V:vpsIicrh?";
static const struct option longOpts[] = {
		{"version", no_argument, NULL,'v'},
		{"get-pin-status",no_argument, NULL,'p' },
		{"get-data-status",no_argument, NULL,'s'},
		{"get-imsi",no_argument, NULL,'i'},
		{"get-imei",no_argument, NULL,'I'},
		{"get-ccid",no_argument, NULL,'c'},
		{"get-rssi",no_argument, NULL,'r'},
		{"verify-pin",required_argument, NULL,'V'},
		{"help", no_argument, NULL,'h'},
		{ NULL, no_argument, NULL, 0 }
};

void display_usage( void )
{
     printf("Options:\n"
    		"  --version, -v:                 Programm version\n"
    		"  --get-pin-status, -p:          Get PIN status\n"
    		"  --get-data-status, -s:         Get data connection status\n"
    		"  --get-imsi, -i:                Get IMSI SIM card\n"
    		"  --get-imei, -I:                Get IMEI modem\n"
    		"  --get-ccid, -c:                Get ICCID SIM card\n"
    		"  --get-rssi, -r:                Get RSSI level\n"
    		"  --verify-pin <pin>, -V <pin>:  Verify PIN\n"
    		);
    exit( EXIT_FAILURE );
}
/*
 * {
        "pin1_status": "disabled",
        "pin1_verify_tries": 3,
        "pin1_unblock_tries": 10,
        "pin2_status": "not_verified",
        "pin2_verify_tries": 3,
        "pin2_unblock_tries": 10
}
{
        "pin1_status": "verified",
        "pin1_verify_tries": 3,
        "pin1_unblock_tries": 10,
        "pin2_status": "not_verified",
        "pin2_verify_tries": 3,
        "pin2_unblock_tries": 10
}
 */

int main( int argc, char *argv[] )
{
  int opt = 0;
  int longIndex;
  int temp;


 opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
 while( opt != -1 ) {
	 switch( opt ) {
	 case 'v':
		 printf(VERSION);
		 break;
	 case 'p':
		 temp = getPinState();
		 if(!temp){
			 printf("UIM uninitialized\n");
		 }else{
			 printf("{\n");
			 printf(" \"pin1_status\": ");
			 if(temp == 1){
				 printf("verified,\n");
			 }else{
				printf("not_verifed,\n");
			 }
			 printf(" \"pin1_verify_tries\": \"error\",\n");
			 printf(" \"pin1_unblock_tries\": \"error\",\n");
			 printf(" \"pin2_status\": \"error\",\n");
			 printf(" \"pin1_verify_tries\": \"error\",\n");
			 printf(" \"pin2_unblock_tries\": \"error\",\n");
			 printf("}\n");
		 }
		 break;
	 case 's':
		 if(GetStatusConnection()){
			 printf("\"connected\"\n");
		 }else{
			 printf("\"disconnected\"\n");
		 }
		 break;
	 case 'i':
		 get_IMSI();
		 break;
	 case 'I':
		 return GetIMEI();
		 break;
	 case 'c':
		 get_ICCID();
		 break;
	 case 'r':
		 get_RSSI();
		 break;
	 case 'V':
		  return VerifyPIN(optarg);
		 break;
	 case 'h':
	 case '?':
		 display_usage();
		 break;
	 default:
		 printf("Error options\n");
		 break;
	 }
	 opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
 }

}
