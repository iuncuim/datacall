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
		{"verify-pin",required_argument, NULL,'r'},
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
