#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "gui_model.h"

/* Gtk widgets that need to be accessed outside of Gtk thread */
GtkWidget *pImage;
GtkWidget *pChangePwdDialog;
GtkWidget *pPasswordDialog;
GtkWidget *pInvalidPwdDialog;	
GtkWidget *window;
GtkWidget *pPasswordMismatchDialog;

GladeXML *gxml;

/* GUI model */
PGUIMODEL pGuiModel;

/* Global for quitting the frame grabber */
int quit;

#endif
