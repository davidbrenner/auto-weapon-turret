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
#include <sched.h>
#include "Global.h"
#include "serial.h"
#include "joy_stick.h"
#include "translate.h"



// need to include /usr/include/linux/joystick.h
#include <linux/joystick.h>

#define MAX_AXIS 16
#define MAX_BUTTON 16

/* Struct needed for C/C++ compatability */
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



void *joy_thread_function(void * ptr);
void *fire_thread_function(void * ptr);
void *x_axis_thread_function(void * ptr);
void *y_axis_thread_function(void * ptr);

padData pad;
FILE * fd;
char * out;

static pthread_t joy_thread, x_axis_thread, y_axis_thread, fire_thread;

int joy_stick_init (void)
{	
    int jt_ret, xt_ret, yt_ret, ft_ret;

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
    } else {
        fprintf (stderr,"failed to open /dev/input/js0\n");
        return -1;
    }
    jt_ret = pthread_create( &joy_thread, NULL, joy_thread_function, NULL);
    xt_ret = pthread_create( &x_axis_thread, NULL, x_axis_thread_function, NULL);
    yt_ret = pthread_create( &y_axis_thread, NULL, y_axis_thread_function, NULL);
    ft_ret = pthread_create( &y_axis_thread, NULL, fire_thread_function, NULL);
    return 0;
} 

void joy_stick_cleanup(void)
{
    pthread_join( joy_thread, NULL );
    pthread_join( x_axis_thread, NULL );
    pthread_join( y_axis_thread, NULL );
    pthread_join( fire_thread, NULL );
    close(pad.fd);
}

void *joy_thread_function(void * ptr)
{
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
        } else sched_yield();;


        if (pad.changed)
        {
            /*int i = 0;
               printf ("axis : %d\n", pad.axisCount);
               printf ("buttons : %d\n", pad.buttonCount);
               printf ("version : %d\n", pad.version);
               printf ("name : %s\n", pad.devName);
               printf ("----------------------------------------------\n");
               printf ("last ev time : %d\n", pad.ev.time);
               for (i=0;i<pad.axisCount;i++) printf (" Axis %2d |",i);
               printf ("\n");
               for (i=0;i<pad.axisCount;i++) printf (" %7d |",pad.aPos[i]);
               printf ("\n\n");
               for (i=0;i<pad.buttonCount;i++) printf (" Btn.%2d |",i);
               printf ("\n");
               for (i=0;i<pad.buttonCount;i++) printf (" %2d |",pad.bPos[i]);
               printf ("\n");*/
            pad.changed = 0;
        }
    }
    return 0;
}

void *x_axis_thread_function(void * ptr)
{
    int sleep_t;
    int tmpPos;
    printf("---x thread started\n");
    while(!quit)
    {
		if((pGuiModel->cStatus & MODE) == USER)
		{
    		tmpPos = pad.aPos[0];
    		if(tmpPos !=0){
        		sleep_t = -14.893*abs(tmpPos)+ 500015;
        		if(tmpPos < 0){
            		if(servo.xPos > MIN_X_PWM){
                		servo.xPos--;
            		}
        		}else if(tmpPos > 0){
            		if(servo.xPos < MAX_X_PWM){
              		  servo.xPos++;
            		}
        		}
            //printf("===x_axis moving\n");
            //printf("x_axis current position: %d\n",servo.xPos);
            //printf("y_axis current position: %d\n",servo.yPos);

        		move_x(servo.xPos);
        		usleep(sleep_t);
    		}
    		sched_yield();
		}
		else
		{
			sched_yield();
		}
    }
    return 0;
}

void *fire_thread_function(void * ptr)
{
    int firing = 0;
	printf("---fire thread started\n");
    while(!quit)
    {
		if((pGuiModel->cStatus & MODE) == USER)
		{
			if(pad.bPos[0] == 1 && !firing)
		    {
				printf("\r\ncStatus: %2x", pGuiModel->cStatus);
				switch(pGuiModel->cStatus & CALIBRATE)
				{
					case S1:
						//printf("\r\nMAX Y PWM CAL: %d", servo.yPos);
						calibrate_y_max(servo.yPos);
						pGuiModel->cStatus &= ~CALIBRATE;
						pGuiModel->cStatus |= S2;
						break;
					case S2:
				        //printf("\r\nMAX X PWM CAL: %d", servo.xPos);
						calibrate_x_max(servo.xPos);
						pGuiModel->cStatus &= ~CALIBRATE;
						pGuiModel->cStatus |= S3;
						break;
					case S3:
						//printf("\r\nMIN Y PWM CAL: %d", servo.yPos);
						calibrate_y_min(servo.yPos);
						pGuiModel->cStatus &= ~CALIBRATE;
						pGuiModel->cStatus |= S4;
						break;
					case S4:
						//printf("\r\nMIN X PWM CAL: %d", servo.xPos);
						calibrate_x_min(servo.xPos);
						break;
					default:
		    			fire();
						break;
				}
		        firing = 1;
		    }
		    else if(pad.bPos[0] == 0 && firing)
		    {
		        firing = 0;
		    }
		    sched_yield();
		}
		else
		{
			sched_yield();
		}
    }
    return 0;
}

void *y_axis_thread_function(void * ptr)
{
    int sleep_t;
    int tmpPos;
    printf("---y thread started\n");
    while(!quit)
    {
		if((pGuiModel->cStatus & MODE) == USER)
		{	
		    tmpPos = pad.aPos[1];
		    if(tmpPos !=0){
		        sleep_t = -14.893*abs(tmpPos)+ 500015;
		        if(tmpPos > 0){
		            if(servo.yPos > MIN_Y_PWM){
		                servo.yPos--;
		            }
		        }else if(tmpPos < 0){
		            if(servo.yPos < MAX_Y_PWM){
		                servo.yPos++;
		            }
		        }

		        //            printf("===y_axis moving\n");
		        //            printf("x_axis current position: %d\n",servo.xPos);
		        //            printf("y_axis current position: %d\n",servo.yPos);

		        move_y(servo.yPos);
		        usleep(sleep_t);
		    }
		    sched_yield();
		}
		else
		{
			sched_yield();
		}
    }
    return 0;
}
