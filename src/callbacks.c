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
#include <stdio.h>

#include "callbacks.h"
#include "Global.h"

void on_window_main_destroy(GtkObject *object, gpointer user_data)
{
	quit = 1;
	gtk_main_quit();
}

void on_btn_change_pwd_clicked(GtkButton *button, gpointer user_data)
{
	printf("is this working?");
	gtk_widget_show_all(pChangePwdDialog);
}


