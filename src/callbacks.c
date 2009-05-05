/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.c
 * Copyright (C) David Brenner, Cory Merkel 2009 <davidb@csh.rit.edu, cem1103@rit.edu>
 * 
 * callbacks.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * callbacks.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>

#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include <glade/glade.h>
#include <stdio.h>

#include "Global.h"
#include "callbacks.h"
#include "password.h"

/* Private helpers */
static void ClearChangePWData(void);
static void ClearPWData(void);

/* Private variables */
static int bLockPressed = 0;

/* Callbacks */
void on_window_main_destroy(GtkObject *object, gpointer user_data)
{
	quit = 1;
	gtk_main_quit();
}

void on_btn_change_pwd_clicked(GtkButton *button, gpointer user_data)
{
	ClearChangePWData();
	gtk_widget_show(pChangePwdDialog);
	    
}

void on_change_pwd_dialog_close(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(pChangePwdDialog);
}

void on_change_pw_cancel_btn_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(pChangePwdDialog);
}

void on_change_pw_OK_btn_clicked(GtkButton *button, gpointer user_data)
{	
	const char* text;
	/* Check to see if the OLD password is correct */
	if( CheckPassword( gtk_entry_get_text( (GtkEntry*)pOldPWEntry ) ) == 0 )
	{
		/* Check to see if the NEW and CONFIRMED passwords match */
		if( strcmp( gtk_entry_get_text( (GtkEntry*)pNewPWEntry ),
			         gtk_entry_get_text( (GtkEntry*)pConfPWEntry ) ) == 0 )
		{
			
			text = gtk_entry_get_text( (GtkEntry*)pNewPWEntry );
			
			/* Set the new password */
			SetPassword(text);			
			
			gtk_widget_hide(pChangePwdDialog);
		}
		else 
		{
			/* New and Confirmed Passwords do not match */
			gtk_widget_show(pPWMismatchDialog);
		}
	}
	else
	{
		/* Incorrect Password */
		gtk_widget_show(pInvalidPWDialog);
	}
}

void on_pw_missmatch_OK_btn_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(pPWMismatchDialog);
}

void on_invalid_pw_OK_btn_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(pInvalidPWDialog);
}

void on_btn_lock_sys_clicked(GtkButton *button, gpointer user_data)
{
	bLockPressed = 1;
	ClearPWData();
	gtk_widget_show(pPWDialog);
}

void on_pw_cancel_btn_clicked(GtkButton *button, gpointer user_data)
{
	/* See where we're coming from */
	bLockPressed = 0;
	gtk_widget_hide(pPWDialog);
}

void on_pw_OK_btn_clicked(GtkButton *button, gpointer user_data)
{
	/* Check the password */
	if( CheckPassword(gtk_entry_get_text((GtkEntry*)pPWEntry)) == 0)
	{
		if( ( (pGuiModel->cStatus)&LOCK ) == LOCKED )
		{
			pGuiModel->cStatus &= ~LOCKED;
			gtk_button_set_label((GtkButton*)pLockButton, "Lock System");
			gtk_widget_show((GtkWidget*) pTable1);
			gtk_widget_show((GtkWidget*) pCalibrateBtn);
			if((pGuiModel->cStatus & MODE) == USER)
			{
				gtk_window_set_title((GtkWindow*)window, "AWT -- (USER, UNLOCKED)");
			}
			else
			{
				gtk_window_set_title((GtkWindow*)window, "AWT -- (AUTO, UNLOCKED)");
			}
		}
		else
		{
			pGuiModel->cStatus |= LOCKED;
			gtk_button_set_label((GtkButton*)pLockButton, "Unlock System");
			gtk_widget_hide((GtkWidget*) pTable1);
			gtk_widget_hide(pCalibrateBtn);
			if((pGuiModel->cStatus & MODE) == USER)
			{
				gtk_window_set_title((GtkWindow*)window, "AWT -- (USER, LOCKED)");
			}
			else
			{
				gtk_window_set_title((GtkWindow*)window, "AWT -- (AUTO, LOCKED)");
			}
		}
		
		gtk_widget_hide(pPWDialog);
	}
	else
	{
		gtk_widget_show(pInvalidPWDialog);
	}
}

void on_rb_auto_clicked(GtkButton *button, gpointer user_data)
{
	pGuiModel->cStatus |= AUTO;
	gtk_widget_hide(pCalibrateBtn);
	gtk_window_set_title((GtkWindow*)window, "AWT -- (AUTO, UNLOCKED)");
	//gtk_widget_set_sensitive(pCalibrateBtn, FALSE);
	
}

void on_rb_user_clicked(GtkButton *button, gpointer user_data)
{
	pGuiModel->cStatus &= ~AUTO;
	gtk_widget_show(pCalibrateBtn);
    pthread_mutex_lock( &joystick_mode_mutex );
    printf("broadcasting to joystick code\n");
    pthread_cond_broadcast( &joystick_mode_cond );
    pthread_mutex_unlock( &joystick_mode_mutex );
	gtk_window_set_title((GtkWindow*)window, "AWT -- (USER, UNLOCKED)");
	//gtk_widget_set_sensitive(pCalibrateBtn, TRUE);
}

void on_calibrate_btn_clicked(GtkButton *button, gpointer user_data)
{
	if((pGuiModel->cStatus & CALIBRATE) == DONE)
	{
		gtk_button_set_label((GtkButton*)pCalibrateBtn, "Done");
		gtk_widget_hide((GtkWidget*) pTable1);
		gtk_widget_hide((GtkWidget*) pCngPwButton);
		gtk_widget_hide((GtkWidget*) pLockButton);
		pGuiModel->cStatus |= CALIBRATE;
	    gtk_widget_show((GtkWidget*) pCalText);
	}
	else
	{
		//calibrate(x_pix_cal, y_pix_cal, x_pwm_cal, y_pix_cal, cur_cal);
		gtk_button_set_label((GtkButton*)pCalibrateBtn, "Calibrate");
		gtk_widget_show((GtkWidget*) pTable1);
		gtk_widget_show((GtkWidget*) pCngPwButton);
		gtk_widget_show((GtkWidget*) pLockButton);
		pGuiModel->cStatus &= ~CALIBRATE;
		gtk_widget_hide((GtkWidget*) pCalText);
	}
}

/* Helper functions */
void ClearChangePWData(void)
{
	gtk_entry_set_text((GtkEntry*)pOldPWEntry, "");
	gtk_entry_set_text((GtkEntry*)pNewPWEntry, "");
	gtk_entry_set_text((GtkEntry*)pConfPWEntry, "");
}

void ClearPWData(void)
{
	gtk_entry_set_text((GtkEntry*)pPWEntry, "");
}
