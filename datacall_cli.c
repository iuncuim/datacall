#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct globalArgs_t {
  int getPinStatus;    /* -p */
  int getDataStatus;   /* -s */
  int getIMSI;         /* -i */
  int getIMEI;         /* -I */
  int getICCID;        /* -c */
  int getRSSI;         /* -r */
  char *getARP;        /* -a */
  int help;            /* -h */
} globalArgs;
 
static const char *optString = "psIicra:h?";

void display_usage( void )
{
    puts( "datacall_cli - info about datacall" );
    /* ... */
    exit( EXIT_FAILURE );
}

extern int get_stre();
extern int get_IMSI();
extern int get_ICCID();

int main( int argc, char *argv[] )
{
  int opt = 0;
  globalArgs.getPinStatus = 0;     
  globalArgs.getDataStatus = 0;
  globalArgs.getIMSI = 0;
  globalArgs.getIMEI = 0;
  globalArgs.getICCID = 0;
  globalArgs.getRSSI = 0;
  globalArgs.getARP = NULL;

  opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
      switch( opt ) {
        case 'p':
          globalArgs.getPinStatus = 1;
          break;

        case 's':
          globalArgs.getDataStatus = 1;
          break;
                 
        case 'i':
          globalArgs.getIMSI = 1;
          get_IMSI();
          break;

        case 'I':
          globalArgs.getIMEI = 1;
          break;
           
        case 'c':
          globalArgs.getICCID = 1;
          get_ICCID();
          break;

        case 'r':
          globalArgs.getRSSI = 1;
          get_RSSI();
          break;

        case 'a':
          globalArgs.getARP = optarg;
          printf("Handle is: %s\n", globalArgs.getARP);
          break;
                 
        case 'h':   
        case '?':
          display_usage();
          break;
                 
        default:
          break;
        }         
        opt = getopt( argc, argv, optString );
  }

}