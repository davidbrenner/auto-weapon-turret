/*******************************************************
 * translate.c
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * 
 *
 *
 *******************************************************/
#include "translate.h"
#include "Global.h"

 
/* Private Variables */
static double mx;
static double my;
static double bx;
static double by;

/* This function performs a linear interpolation estimation using */
/* the least-squares approximation with the passed data.          */
void calibrate(int* piXpxl,
               int* piYpxl,
               int* piXpwm,
               int* piYpwm,
               int  iSize)
{
    double SXx  = 0;
    double SXxx = 0;
    double SXxy = 0;
    double SXy  = 0;
    double SYx  = 0;
    double SYxx = 0;
    double SYxy = 0;
    double SYy  = 0;
    int i;

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

    mx = (iSize*SXxy-SXx*SXy)/(iSize*SXxx-SXx*SXx);
    bx = (SXxx*SXy-SXxy*SXx)/(iSize*SXxx-SXx*SXx);


    my = (iSize*SYxy-SYx*SYy)/(iSize*SYxx-SYx*SYx);
    by = (SYxx*SYy-SYxy*SYx)/(iSize*SYxx-SYx*SYx);


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
