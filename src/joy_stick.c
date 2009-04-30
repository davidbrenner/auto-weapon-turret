// code from: http://coding.derkeiler.com/Archive/General/comp.programming/2007-05/msg00480.html

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <string.h>
#include "joy_stick.h"
#include "Global.h"

#define  OPEN_ERROR 1
#define BAUDRATE B9600


// need to include /usr/include/linux/joystick.h
#include <linux/joystick.h>

#define MAX_AXIS 16
#define MAX_BUTTON 16

#define MIN_X_PWM 920
#define MAX_X_PWM 1750
#define MIN_Y_PWM 980
#define MAX_Y_PWM 1850


typedef struct js_eventA {
    __u32 time; /* event timestamp in milliseconds */
    __s16 value;    /* value */
    __u8 type;  /* event type */
    __u8 number;    /* axis/button number */
} js_event;




typedef struct padDataA {
    unsigned char axisCount;
    unsigned char buttonCount;
    int fd;
    int version;
    char devName[80];
    int aPos[MAX_AXIS];
    int bPos[MAX_BUTTON];
    int changed;
    js_event ev;
} padData;

typedef struct servosA {
    int xPos;
    int yPos;
} servos;


void *joy_thread_function(void * ptr);
void *fire_thread_function(void * ptr);
void *x_axis_thread_function(void * ptr);
void *y_axis_thread_function(void * ptr);
int SerialInit(const char* pcDev, int iBaudRate, int* iSerPort);


int serial_fd;
padData pad;
servos servo;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
FILE * fd;
char * out;

static pthread_t joy_thread, x_axis_thread, y_axis_thread, fire_thread;

void do_it (void)
{	
    int jt_ret, xt_ret, yt_ret, ft_ret;
    const char * serial_port = "/dev/ttyUSB0";
    out = (char *) malloc(sizeof(char)*8);
    //const char * serial_port = "/dev/pts/2";
    SerialInit(serial_port, 9600, &serial_fd);
    fd = fdopen(serial_fd, "rw");

    pad.fd = open("/dev/input/js0", O_RDONLY);
    if (pad.fd > 0)
    {
		int i;
        printf ("js0 opened OK\n");

        // Get pad info ...
        ioctl(pad.fd, JSIOCGAXES, &pad.axisCount);
        ioctl(pad.fd, JSIOCGBUTTONS, &pad.buttonCount);
        ioctl(pad.fd, JSIOCGVERSION, &pad.version);
        ioctl(pad.fd, JSIOCGNAME(80), &pad.devName);
        fcntl(pad.fd, F_SETFL, O_NONBLOCK);

        printf ("axis : %d\n", pad.axisCount);
        printf ("buttons : %d\n", pad.buttonCount);
        printf ("version : %d\n", pad.version);
        printf ("name : %s\n", pad.devName);

        // set default values
        pad.changed = 0;
        for (i=0;i<pad.axisCount;i++) pad.aPos[i]=0;
        for (i=0;i<pad.buttonCount;i++) pad.bPos[i]=0;
    } else 
    {
        printf ("failed to open /dev/input/js0\n");
    }
    if (serial_fd > 0)
    {
        servo.xPos = MIN_X_PWM+(MAX_X_PWM-MIN_X_PWM)/2;
        servo.yPos = MIN_Y_PWM+(MAX_Y_PWM-MIN_Y_PWM)/2;
        sprintf(out, "#%04d%04d$", servo.xPos, servo.yPos);
        write(serial_fd,out,10);
        tcflush(serial_fd, TCIFLUSH);
        jt_ret = pthread_create( &joy_thread, NULL, joy_thread_function, NULL);
        xt_ret = pthread_create( &x_axis_thread, NULL, x_axis_thread_function, NULL);
        yt_ret = pthread_create( &y_axis_thread, NULL, y_axis_thread_function, NULL);
        ft_ret = pthread_create( &y_axis_thread, NULL, fire_thread_function, NULL); 
		
    } else 
    {
        printf ("failed to open serial port!\n");
    }

} 

void cleanup(void)
{
	pthread_join( joy_thread, NULL );
	pthread_join( x_axis_thread, NULL );
	pthread_join( y_axis_thread, NULL );
	pthread_join( fire_thread, NULL );
	close(serial_fd);
	close(pad.fd);
}

void *joy_thread_function(void * ptr)
{
    //printf("joy thread started\n");
    while (!quit)
    {
        int result = read(pad.fd, &pad.ev, sizeof(pad.ev));
        if (result > 0)
        {
            switch (pad.ev.type)
            {
                case JS_EVENT_INIT:
                case JS_EVENT_INIT | JS_EVENT_AXIS:
                case JS_EVENT_INIT | JS_EVENT_BUTTON:
                    break;
                case JS_EVENT_AXIS:
                    pad.aPos[pad.ev.number] = pad.ev.value;
                    pad.changed = 1;
                    break;
                case JS_EVENT_BUTTON:
                    pad.bPos[pad.ev.number] = pad.ev.value;
                    pad.changed = 1;
                    break;
                default:
                    printf ("Other event ? %d\nnumber=%d\nvalue=%d\n",
                            pad.ev.type,pad.ev.number, pad.ev.value);
                    break;
            }
        } else usleep(1);


        if (pad.changed)
        {
            printf ("axis : %d\n", pad.axisCount);
            printf ("buttons : %d\n", pad.buttonCount);
            printf ("version : %d\n", pad.version);
            printf ("name : %s\n", pad.devName);
            printf ("----------------------------------------------\n");
            printf ("last ev time : %d\n", pad.ev.time);

            for (int i=0;i<pad.axisCount;i++) printf (" Axis %2d |",i);
            printf ("\n");
            for (int i=0;i<pad.axisCount;i++) printf (" %7d |",pad.aPos[i]);
            printf ("\n\n");
            for (int i=0;i<pad.buttonCount;i++) printf (" Btn.%2d |",i);
            printf ("\n");
            for (int i=0;i<pad.buttonCount;i++) printf (" %2d |",pad.bPos[i]);
            printf ("\n");
            pad.changed = 0;
        }
    }

}

void *x_axis_thread_function(void * ptr)
{
    //printf("x thread started\n");
    int sleep_t;
    while(!quit)
    {
        int delta = 0;
        if(pad.aPos[0] !=0){
            //sleep_t = -2.8688*abs(pad.aPos[0])+ 100003;
            sleep_t = -14.893*abs(pad.aPos[0])+ 500015;
            if(pad.aPos[0] < 0){
                if(servo.xPos > MIN_X_PWM){
                    servo.xPos--;
                }
            }else if(pad.aPos[0] > 0){
                if(servo.xPos < MAX_X_PWM){
                    servo.xPos++;
                }
            }
            //printf("===x_axis moving\n");
            //printf("x_axis current position: %d\n",servo.xPos);
            //printf("y_axis current position: %d\n",servo.yPos);

            pthread_mutex_lock( &mutex1 );
            // do serial i/o here
            //fprintf(fd, "%03d\n\r", servo.xPos);
            //sprintf(out, "%04d\n", servo.xPos);
            //write(serial_fd,out,5);
            //tcflush(serial_fd, TCIFLUSH);
            sprintf(out, "#%04d$", servo.xPos);
            write(serial_fd,out,6);
            tcflush(serial_fd, TCIFLUSH);
            pthread_mutex_unlock( &mutex1 );
            usleep(sleep_t);
        }
    }
}

void *fire_thread_function(void * ptr)
{
    int firing = 0;
    while(!quit)
    {
        if(pad.bPos[0] == 1 && !firing)
        {
            pthread_mutex_lock( &mutex1 );
            write(serial_fd,"#F$",4);
            tcflush(serial_fd, TCIFLUSH);
            pthread_mutex_unlock( &mutex1 );
            firing = 1;
        }
        else if(pad.bPos[0] == 0 && firing)
        {
            //pthread_mutex_lock( &mutex1 );
            //write(serial_fd,"#OF$",4);
            //tcflush(serial_fd, TCIFLUSH);
            //pthread_mutex_unlock( &mutex1 );
            firing = 0;
        }
        usleep(1);
    }
}

void *y_axis_thread_function(void * ptr)
{
    //printf("y thread started\n");
    int sleep_t;
    while(!quit)
    {
        int delta = 0;
        if(pad.aPos[1] !=0){
            sleep_t = -14.893*abs(pad.aPos[1])+ 500015;
            if(pad.aPos[1] > 0){
                if(servo.yPos > MIN_Y_PWM){
                    servo.yPos--;
                }
            }else if(pad.aPos[1] < 0){
                if(servo.yPos < MAX_Y_PWM){
                    servo.yPos++;
                }
            }
            //printf("===y_axis moving\n");
            //printf("x_axis current position: %d\n",servo.xPos);
            //printf("y_axis current position: %d\n",servo.yPos);

            pthread_mutex_lock( &mutex1 );
            // do serial i/o here
            //fprintf(fd, "y%03d\n\r", servo.yPos);
            //sprintf(out, "y%04d\n", servo.yPos);
            //write(serial_fd,out,6);
            //tcflush(serial_fd, TCIFLUSH);
            sprintf(out, "#y%04d$", servo.yPos);
            write(serial_fd,out,7);
            tcflush(serial_fd, TCIFLUSH);
            pthread_mutex_unlock( &mutex1 );
            usleep(sleep_t);
        }
    }
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

}
