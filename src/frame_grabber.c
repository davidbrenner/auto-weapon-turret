#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include "Global.h"
#include "frame_grabber.h"
#include <pthread.h>

IplImage *gtkMask;
IplImage* frame;


// Converts OpenCV image to GTK pixbuf
// Sets pImage to pixbuf containing gtkMask (openCV) image
void convertOpenCv2Gtk(IplImage *image){
    GdkPixbuf *pix;
    cvCvtColor( image, gtkMask, CV_BGR2RGB ); // Usually opencv image is BGR, so we need to change it to RGB
    pix = gdk_pixbuf_new_from_data ((guchar*)gtkMask->imageData,
            GDK_COLORSPACE_RGB,
            FALSE,
            gtkMask->depth,
            gtkMask->width,
            gtkMask->height,
            (gtkMask->widthStep),
            NULL,
            NULL);
    gtk_image_set_from_pixbuf( (GtkImage *) pImage, pix );
} 

// Updates the GTK image widget with the current frame
gboolean time_handler(GtkWidget *widget)
{
	pthread_mutex_lock(&frame_grab_mutex);
	if(!quit){
		gtk_widget_queue_draw (pImage);
		pthread_mutex_unlock(&frame_grab_mutex);
		return TRUE;
	}else
		pthread_mutex_unlock(&frame_grab_mutex);
		return FALSE;
} 

// Main function for the frame grabber thread
// Grabs a frame from the webcam, displays it on the GTK image widget
void *frame_grabber(void * ptr)
{
    CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
    if( !capture ) {
        fprintf( stderr, "ERROR: capture is NULL \n" );
        getchar();
        return NULL;
    }

    // Grab a frame
    frame = cvQueryFrame( capture );
    if( !frame ) {
        fprintf( stderr, "ERROR: frame is null...\n" );
    }else {
        // Convert it to a GTK image
		gtkMask = cvCreateImage( cvGetSize(frame), 8, 3);
        convertOpenCv2Gtk(frame);
    }

    while( !quit ) {
		
		pthread_mutex_lock(&frame_grab_mutex);
		
        // Get one frame
        frame = cvQueryFrame( capture );
        if( !frame ) {
            fprintf( stderr, "ERROR: frame is null...\n" );
            break;
        }
		cvCvtColor( frame, gtkMask, CV_BGR2RGB );
		
		pthread_mutex_unlock(&frame_grab_mutex);
    }

    // Release the capture device housekeeping
    cvReleaseCapture( &capture );
    cvDestroyWindow( "mywindow" );
    return NULL;
}
