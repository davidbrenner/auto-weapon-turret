/*******************************************************
 * translate.h
 *
 * Author(s): David Brenner, Cory Merkel
 *
 *
 *
 *
 *******************************************************/
 #ifndef __TRANSLATE_H__
 #define __TRANSLATE_H__

#ifdef __cplusplus
extern "C" {
#endif
 
 /* Global Functions */
 void calibrate_x_min(int iWidth);
 void calibrate_x_max(int iWidth);
 void calibrate_y_min(int iWidth);
 void calibrate_y_max(int iWidth);
 void set_max_x_coordinate(int iMax);
 void set_max_y_coordinate(int iMax);
 
 int x_pix_to_pwm(int iCoordinate);
 int y_pix_to_pwm(int iCoordinate);
#ifdef __cplusplus
}
#endif
 
 #endif
