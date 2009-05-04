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
static int calibrated_x_min = 1205; 
static int calibrated_x_max = 1533;
static int calibrated_y_min = 1416;
static int calibrated_y_max = 1240;
static int max_x_coordinate = 640;
static int max_y_coordinate = 480;

/* Mutators */
void calibrate_x_min(int iWidth)
{
	calibrated_x_min = iWidth;
}

void calibrate_x_max(int iWidth)
{
	calibrated_x_max = iWidth;
}

void calibrate_y_min(int iWidth)
{
	calibrated_y_min = iWidth;
}

void calibrate_y_max(int iWidth)
{
	calibrated_y_max = iWidth;
}

void set_max_x_coordinate(int iMax)
{
	max_x_coordinate = iMax;
}

void set_max_y_coordinate(int iMax)
{
	max_y_coordinate = iMax;
}
 
/* Functions to translate coordinates to pwm widths */
int x_pix_to_pwm(int iCoordinate)
{
	double percent = (double)iCoordinate/max_x_coordinate;
	double range   = (double)calibrated_x_max - (double)calibrated_x_min;
	return (int) (percent*range)+calibrated_x_min;
}

int y_pix_to_pwm(int iCoordinate)
{
	double percent = (double)iCoordinate/max_y_coordinate;
	double range   = (double)calibrated_y_min - (double)calibrated_y_max;
	return (int) (percent*range)+calibrated_y_max;

}
