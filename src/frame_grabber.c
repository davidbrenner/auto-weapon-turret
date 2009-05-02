#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include "Global.h"
#include "frame_grabber.h"
#include <pthread.h>

// Updates the GTK image widget with the current frame
gboolean time_handler(GtkWidget *widget)
{
	if(!quit){
		if(pImage){
            gtk_widget_queue_draw (pImage);
        }
		return TRUE;
	}else
		return FALSE;
} 
