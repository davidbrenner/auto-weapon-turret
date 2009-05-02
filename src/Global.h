#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "cv.h"
#include "highgui.h"
#include <gtk/gtk.h>
#include <glade/glade.h>
#include "gui_model.h"
#include <pthread.h>

#define MIN_X_PWM 920
#define MAX_X_PWM 1750
#define MIN_Y_PWM 980
#define MAX_Y_PWM 1850

/* Gtk widgets that need to be accessed outside of Gtk thread */
GtkWidget *pImage;
GtkWidget *pChangePwdDialog;
GtkWidget *pPasswordDialog;
GtkWidget *pInvalidPwdDialog;	
GtkWidget *window;
GtkWidget *pPasswordMismatchDialog;
GtkWidget *pOldPWEntry;
GtkWidget *pNewPWEntry;
GtkWidget *pConfPWEntry;
GtkWidget *pPWMismatchDialog;
GtkWidget *pInvalidPWDialog;
GtkWidget *pPWDialog;
GtkWidget *pPWEntry;
GtkWidget *pLockButton;
GtkWidget *pUserButton;
GtkWidget *pAutoButton;
GtkWidget *pTable1;
GtkWidget *pFixed1;

GladeXML *gxml;

/* GUI model */
PGUIMODEL pGuiModel;

/* Global for quitting the frame grabber */
int quit;

// Global mutex for serial access
pthread_mutex_t serial_mutex;

// Current servo position
typedef struct servosA {
    int xPos;
    int yPos;
} servos;

servos servo;

char draw_ready;

#endif
