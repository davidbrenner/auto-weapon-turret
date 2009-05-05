/*******************************************************
 * translate.c
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * 
 *
 *
 *******************************************************/
#include <stdio.h>
#include "translate.h"
#include "Global.h"

 
/* Private Variables */
static const char* PW_FILE = "calibration.conf";
static double mx;
static double my;
static double bx;
static double by;

void translate_init(void){
	FILE* fp = fopen(PW_FILE, "r");
    if(fp){
        fscanf(fp,"%lf", &mx);
        fscanf(fp,"%lf", &bx);
        fscanf(fp,"%lf\n", &my);
        fscanf(fp,"%lf\n", &by);
        printf("scanned calibration values: mx: %f bx: %f my: %f by %f\n",mx,bx,my,by);
        fclose(fp);
    }
}

/* This function performs a linear interpolation estimation using */
/* the least-squares approximation with the passed data.          */
void calibrate(int* piXpxl,
        int* piYpxl,
        int* piXpwm,
        int* piYpwm,
        int  iSize)
{
    FILE* fp = fopen(PW_FILE, "w");
    double SXx  = 0;
    double SXxx = 0;
    double SXxy = 0;
    double SXy  = 0;
    double SYx  = 0;
    double SYxx = 0;
    double SYxy = 0;
    double SYy  = 0;
    int i;

    printf("calibrate called with size: %d", iSize);

    for(i = 0; i < iSize; ++i)
    {
        SXx  += piXpxl[i];
        SXxx += piXpxl[i]*piXpxl[i];
        SXxy += piXpxl[i]*piXpwm[i];
        SXy  += piXpwm[i];

        SYx  += piYpxl[i];
        SYxx += piYpxl[i]*piYpxl[i];
        SYxy += piYpxl[i]*piYpwm[i];
        SYy  += piYpwm[i];
    }

    printf("SXx: %f SXxx: %f SXxy: %f SXy: %f\n", SXx, SXxx, SXxy, SXy);
    printf("SYx: %f SYxx: %f SYxy: %f SYy: %f\n", SYx, SYxx, SYxy, SYy);

    mx = (iSize*SXxy-SXx*SXy)/(iSize*SXxx-SXx*SXx);
    bx = (SXxx*SXy-SXxy*SXx)/(iSize*SXxx-SXx*SXx);


    my = (iSize*SYxy-SYx*SYy)/(iSize*SYxx-SYx*SYx);
    by = (SYxx*SYy-SYxy*SYx)/(iSize*SYxx-SYx*SYx);


    if(fp)
    {
        printf("writing to file!!\n");
        fprintf(fp,"%f\n",mx);
        fprintf(fp,"%f\n",bx);
        fprintf(fp,"%f\n",my);
        fprintf(fp,"%f\n",by);
        printf("mx: %f bx: %f my: %f by %f\n",mx,bx,my,by);
        fclose(fp);
    }else {
        fprintf(stderr,"Can not print values!\n");
    }
}

/* Functions to translate coordinates to pwm widths */
int x_pix_to_pwm(int iCoordinate)
{
    return (int) (mx*iCoordinate + bx);
}

int y_pix_to_pwm(int iCoordinate)
{
    return (int) (my*iCoordinate + by);
}
