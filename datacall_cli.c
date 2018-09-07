#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "qmiuimdemo.h"


#define VERSION		"Ver: 0.1\n"

 
static const char *optString = "V:vpsIicrh?";
static const struct option longOpts[] = {
		{"version", no_argument, NULL,'v'},
		{"get_pin_status",no_argument, NULL,'p' },
		{"get_data_status",no_argument, NULL,'s'},
		{"get_imsi",no_argument, NULL,'i'},
		{"get_imei",no_argument, NULL,'I'},
		{"get_ccid",no_argument, NULL,'c'},
		{"get_rssi",no_argument, NULL,'r'},
		{"verify_pin",required_argument, NULL,'r'},
		{"help", no_argument, NULL,'h'},
		{ NULL, no_argument, NULL, 0 }
};

void display_usage( void )
{
     printf("Options:\n"
    		"  --version, -v:                 Programm version\n"
    		"  --get_pin_status, -p:          Get PIN status\n"
    		"  --get_data_status, -s:         Get data connection status\n"
    		"  --get_imsi, -i:                Get IMSI SIM card\n"
    		"  --get_imei, -I:                Get IMEI modem\n"
    		"  --get_ccid, -c:                Get ICCID SIM card\n"
    		"  --get_rssi, -r:                Get RSSI level\n"
    		"  --verify_pin <pin>, -V <pin>:  Verify PIN\n"
    		);
    exit( EXIT_FAILURE );
}


int main( int argc, char *argv[] )
{
  int opt = 0;
  int longIndex;


 opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
 while( opt != -1 ) {
	 switch( opt ) {
	 case 'v':
		 printf(VERSION);
		 break;
	 case 'p':
		 getPinState();
		 break;
	 case 's':
		 break;
	 case 'i':
		 get_IMSI();
		 break;
	 case 'I':
		 break;
	 case 'c':
		 get_ICCID();
		 break;
	 case 'r':
		 get_RSSI();
		 break;
	 case 'V':
		 VerifyPIN(optarg);
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
