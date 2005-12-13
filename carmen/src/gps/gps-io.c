#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <values.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <carmen/carmen.h>
#include <carmen/gps_nmea_messages.h>

#include "gps.h"
#include "gps-io.h"

void
DEVICE_init_params( SerialDevice *p )
{
  strncpy( p->ttyport, DEFAULT_GPS_PORT, MAX_NAME_LENGTH );
  p->baud                    = DEFAULT_GPS_BAUD;
  p->parity                  = DEFAULT_GPS_PARITY;
  p->databits                = DEFAULT_GPS_DATABITS;
  p->stopbits                = DEFAULT_GPS_STOPBITS;
  p->hwf                     = DEFAULT_GPS_HWF;
  p->swf                     = DEFAULT_GPS_SWF;
  p->fd                      = -1;
}

int
iParity( enum PARITY_TYPE par )
{
  if (par==NO)
    return(IGNPAR);
  else
    return(INPCK);
}

int
iSoftControl( int flowcontrol )
{
  if (flowcontrol)
    return(IXON);
  else
    return(IXOFF);
}

int
cDataSize( int numbits )
{
  switch(numbits) {
  case 5:
    return(CS5);
    break;
  case 6:
    return(CS6);
    break;
  case 7:
    return(CS7);
    break;
  case 8:
    return(CS8);
    break;
  default:
    return(CS8);
    break;
  }
}

int
cStopSize( int numbits )
{
  if (numbits==2) {
    return(CSTOPB);
  } else {
    return(0);
  }
}

int
cFlowControl( int flowcontrol )
{
  if (flowcontrol) {
    return(CRTSCTS);
  } else {
    return(CLOCAL);
  }
}

int
cParity( enum PARITY_TYPE par )
{
  if (par!=NO) {
    if (par==ODD) {
      return(PARENB | PARODD);
    } else {
      return(PARENB);
    }
  } else {
    return(0);
  }
}

int
cBaudrate( int baudrate )
{
  switch(baudrate) {
  case 0:
    return(B0);
    break;
  case 300:
    return(B300);
    break;
  case 600:
    return(B600);
    break;
  case 1200:
    return(B1200);
    break;
  case 2400:
    return(B2400);
    break;
  case 4800:
    return(B4800);
    break;
  case 9600:
    return(B9600);
    break;
  case 19200:
    return(B19200);
    break;
  case 38400:
    return(B38400);
    break;
  case 57600:
    return(B57600);
    break;
  case 115200:
    return(B115200);
    break;
  case 500000:
    /* to use 500k you have to change the entry of B460800 in you kernel:
       /usr/src/linux/drivers/usb/serial/ftdi_sio.h:
       ftdi_8U232AM_48MHz_b460800 = 0x0006    */
    return(B460800);
    break;
  default:
    return(B9600);
    break;
  }

}

int 
DEVICE_bytes_waiting( int sd )
{
  int available=0;
  if ( ioctl( sd, FIONREAD, &available ) == 0 )
    return available;
  else
    return -1;
}    

void 
DEVICE_set_params( SerialDevice dev )
{
  struct termios  ctio;

  tcgetattr(dev.fd,&ctio); /* save current port settings */

  ctio.c_iflag =
    iSoftControl(dev.swf) |
    iParity(dev.parity);
  ctio.c_oflag = 0;
  ctio.c_cflag =
    CREAD                            |
    cFlowControl(dev.hwf || dev.swf) |
    cParity(dev.parity)              | 
    cDataSize(dev.databits)          |
    cStopSize(dev.stopbits);

  ctio.c_lflag = 0;
  ctio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  ctio.c_cc[VMIN]     = 0;   /* blocking read until 0 chars received */

  cfsetispeed ( &ctio, (speed_t) cBaudrate(dev.baud) );
  cfsetospeed ( &ctio, (speed_t) cBaudrate(dev.baud) );

  tcflush(dev.fd, TCIFLUSH);
  tcsetattr(dev.fd,TCSANOW,&ctio);

}

void 
DEVICE_set_baudrate( SerialDevice dev, int brate )
{
  struct termios  ctio;

  tcgetattr(dev.fd,&ctio); /* save current port settings */

  cfsetispeed ( &ctio, (speed_t) cBaudrate(brate) );
  cfsetospeed ( &ctio, (speed_t) cBaudrate(brate) );

  tcflush(dev.fd, TCIFLUSH);
  tcsetattr(dev.fd,TCSANOW,&ctio);
}

int
DEVICE_connect_port( SerialDevice *dev )
{
  if ( ( dev->fd =
    	 open( (dev->ttyport), (O_RDWR | O_NOCTTY),0) ) < 0 ) {
    return( -1 );
  }
  fprintf( stderr, "." );
  dev->fp = fdopen( dev->fd, "rw" );
  fprintf( stderr, "." );
  if ( dev->fp == NULL ) {
    return( -1 );
  }
  fprintf( stderr, ".\n" );
  fprintf( stderr, "INFO: set device:\n" );
  fprintf( stderr, "INFO:    port   = %s\n", dev->ttyport );
  fprintf( stderr, "INFO:    baud   = %d\n", dev->baud );
  fprintf( stderr, "INFO:    params = %d%s%d\n",
	   dev->databits, dev->parity==NO?"N":dev->parity==ODD?"O":"E",
	   dev->stopbits );

  DEVICE_set_params( *dev );
  return( dev->fd );
}


int
writeData( int fd, unsigned char *buf, int nChars )
{
  int written = 0;
  while (nChars > 0) {
    written = write( fd, buf, nChars );
    if (written < 0) {
      return FALSE;
    } else {
      nChars -= written;
      buf    += written;
    }
    usleep(1000);
  }
  return TRUE;
}

int
DEVICE_send( SerialDevice dev, unsigned char *cmd, int len )
{
#ifdef IO_DEBUG
  int i;
  fprintf( stderr, "\n---> " );
  for (i=0;i<len;i++) {
    fprintf( stderr, "%c", cmd[i] );
  }
  fprintf( stderr, "\n" );
#endif

  if (writeData( dev.fd, cmd, len )) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int
DEVICE_read_data( SerialDevice dev )
{
  int                  val, recieved, len;
  static char        * buffer;
  static size_t        buffer_len = 0;
  static char          line[BUFFER_LENGTH];
  int                  j, start;
  
  val = DEVICE_bytes_waiting(dev.fd);
  if (val>0) {
    recieved = getdelim(&buffer, &buffer_len, '*', dev.fp);
    if (recieved>0) {
      len = 0;
      for (j=0;j<recieved;j++) {
	if (buffer[j]!=0) {
	  line[len] = buffer[j];
	  len++;
	}
      }
      if (len>0) {
	for (j=0;j<len;j++)
	  if (line[j]=='$') break;
	start = j;
	if ( (len-start>0) && (line[len-1]=='*') ) {
#ifdef XXIO_DEBUG
	  fprintf( stderr, "(" );
	  for (j=start;j<len;j++)
	    fprintf( stderr, "%c", line[j] );
	  fprintf( stderr, ")\n" );
#else
	  fprintf( stderr, "." );
#endif
	  return(carmen_gps_parse_data( &(line[start]), len-start ));
	}	
      }
    }
  }
  return(FALSE);
}

