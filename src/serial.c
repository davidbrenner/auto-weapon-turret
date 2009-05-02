#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <string.h>
#include "Global.h"
#include "serial.h"
#include "joy_stick.h"

#define  OPEN_ERROR 1
#define BAUDRATE B9600

int serial_fd;
FILE * fd;
char * out;

int serial_init (void)
{	
    const char * serial_port = "/dev/ttyUSB0";
    out = (char *) malloc(sizeof(char)*8);
    SerialInit(serial_port, 9600, &serial_fd);
    fd = fdopen(serial_fd, "rw");

    if (serial_fd > 0)
    {
        pthread_mutex_t serial_mutex = PTHREAD_MUTEX_INITIALIZER;
        servo.xPos = MIN_X_PWM+(MAX_X_PWM-MIN_X_PWM)/2;
        servo.yPos = MIN_Y_PWM+(MAX_Y_PWM-MIN_Y_PWM)/2;
        sprintf(out, "#%04d%04d$", MIN_X_PWM+(MAX_X_PWM-MIN_X_PWM)/2, MIN_Y_PWM+(MAX_Y_PWM-MIN_Y_PWM)/2);
        write(serial_fd,out,10);
        tcflush(serial_fd, TCIFLUSH);
        return 0;
		
    } else {
        fprintf (stderr,"failed to open serial port!\n");
        return -1;
    }
} 

void serial_cleanup(void)
{
	close(serial_fd);
}


int move_x(int xPos)
{
    int ret = 0;
    if(xPos>MAX_X_PWM || xPos<MIN_X_PWM) return -1;
    pthread_mutex_lock( &serial_mutex );
    sprintf(out, "#%04d$", xPos);
    ret = write(serial_fd,out,6);
    tcflush(serial_fd, TCIFLUSH);
    pthread_mutex_unlock( &serial_mutex );
    return ret <= 0;
}

int fire(void)
{
    int ret = 0;
    pthread_mutex_lock( &serial_mutex );
    ret = write(serial_fd,"#F$",4);
    tcflush(serial_fd, TCIFLUSH);
    pthread_mutex_unlock( &serial_mutex );
    return ret <= 0;
}

int move_y(int yPos)
{
    int ret = 0;
    if(yPos>MAX_Y_PWM || yPos<MIN_Y_PWM) return -1;
    pthread_mutex_lock( &serial_mutex );
    sprintf(out, "#y%04d$", yPos);
    ret = write(serial_fd,out,7);
    tcflush(serial_fd, TCIFLUSH);
    pthread_mutex_unlock( &serial_mutex );
    return ret <= 0;
}

int SerialInit(const char* pcDev, int iBaudRate, int* iSerPort)
{
    struct termios newtio;           // Terminal settings


    /* Open the serial port */
    if( (*iSerPort = open(pcDev, O_RDWR|O_NOCTTY|O_NDELAY)) < 0 )
    { /* ERROR */
        perror(pcDev);
        return OPEN_ERROR;
    }


    /* Clear struct for new port settings */
    memset(&newtio,0, sizeof(newtio));

    /* Set control flags */
    newtio.c_cflag = BAUDRATE|CS8|CLOCAL|CREAD;

    /* Do some other stuff */
    newtio.c_iflag = IGNPAR|ICRNL;

    /* Raw output */
    newtio.c_oflag = 0;

    /* Disable echoing */
    newtio.c_lflag = ICANON;

    /* 
       initialize all control characters 
       default values can be found in /usr/include/termios.h, and are given
       in the comments, but we don't need them here
       */
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    /* Flush and activate */
    tcflush(*iSerPort, TCIFLUSH);
    tcsetattr(*iSerPort, TCSANOW, &newtio);

    return 0;
}
