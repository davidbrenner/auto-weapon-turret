/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) David Brenner, Cory Merkel 2009 <davidb@csh.rit.edu, cem1103@rit.edu>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

#include <pthread.h>
#include "cv.h"
#include "highgui.h"

#include "Global.h"
#include "callbacks.h"
#include "frame_grabber.h"
#include "callbacks.h"
#include "gui_model.h"



/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif






/* For testing propose use the local (not installed) glade file */
/* #define GLADE_FILE PACKAGE_DATA_DIR"/awt/glade/awt.glade" */
#define GLADE_FILE "awt.glade"
	
GtkWidget*
create_window (void)
{
	
	gxml = glade_xml_new (GLADE_FILE, NULL, NULL);
	
	/* This is important */
	glade_xml_signal_autoconnect (gxml);
	
	/* Grab all of the xml widget references */
	window            = glade_xml_get_widget(gxml, "window_main");
	pImage            = glade_xml_get_widget(gxml, "image1");
	pChangePwdDialog  = glade_xml_get_widget(gxml, "change_pwd_dialog");
	pPasswordDialog   = glade_xml_get_widget(gxml, "password_dialog");
	pInvalidPwdDialog = glade_xml_get_widget(gxml, "invalid_pw_dialog");
	pPasswordMismatchDialog = glade_xml_get_widget(gxml, "pw_missmatch_dialog");
	
	return window;
}

int
main (int argc, char *argv[])
{
	
	/* Setup the GUI Model */
	pGuiModel = NewGUIModel();
	
	if(pGuiModel == NULL)
	{
		printf("ERROR: Could not allocate GUI Model");
		return 1;
	}
	
 	GtkWidget *window;
	pthread_t frame_grabber_thread;
	


#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	g_thread_init(NULL);
	gdk_threads_init();
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	window = GTK_WIDGET(create_window ());
	gtk_widget_show (window);
	
	quit = 0;
	pthread_create( &frame_grabber_thread, NULL, frame_grabber, NULL );
	
	g_timeout_add( 50, (GtkFunction)time_handler, NULL );
	gtk_main ();
	
	return 0;
}
