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

/* Translate functions */
void translate_init(void);
 
/* Global Functions */
void calibrate(int* piXpix,
               int* piYpix,
               int* piXpwm,
               int* piYpwm,
               int  iSize);

int x_pix_to_pwm(int iCoordinate);
int y_pix_to_pwm(int iCoordinate);
#ifdef __cplusplus
}
#endif
 
 #endif
