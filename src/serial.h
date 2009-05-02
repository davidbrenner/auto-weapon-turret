/*******************************************************
 * serial.h
 *
 * Author(s): David Brenner, Cory Merkel
 *******************************************************/
#ifndef __SERIAL_H__
#define __SERIAL_H__

/* Global functions */
int serial_init (void);
void serial_cleanup(void);
int move_x(int xPos);
int fire(void);
int move_y(int yPos);

/* local functions */
int SerialInit(const char* pcDev, int iBaudRate, int* iSerPort);

#endif
