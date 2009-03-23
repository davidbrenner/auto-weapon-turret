#ifndef __FRAME_GRABBER_H__
#define __FRAME_GRABBER_H__
#include "cv.h"
#include "highgui.h"
// Converts OpenCV image to GTK pixbuf
// Sets pImage to pixbuf containing gtkMask (openCV) image
void convertOpenCv2Gtk(IplImage* image);

// Updates the GTK image widget with the current frame
gboolean time_handler(GtkWidget *widget);

// Main function for the frame grabber thread
// Grabs a frame from the webcam, displays it on the GTK image widget
void *frame_grabber(void * ptr);

#endif