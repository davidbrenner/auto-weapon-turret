#ifndef __FRAME_GRABBER_H__
#define __FRAME_GRABBER_H__
#include "cv.h"
#include "highgui.h"

// Updates the GTK image widget with the current frame
gboolean time_handler(GtkWidget *widget);

#endif
