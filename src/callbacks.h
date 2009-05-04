/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.h
 * Copyright (C) David Brenner, Cory Merkel 2009 <davidb@csh.rit.edu, cem1103@rit.edu>
 * 
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * callbacks.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

void on_window_main_destroy(GtkObject *object, gpointer user_data);
void on_btn_change_pwd_clicked(GtkButton *button, gpointer user_data);
void on_change_pwd_dialog_close(GtkButton *button, gpointer user_data);
void on_change_pw_cancel_btn_clicked(GtkButton *button, gpointer user_data);
void on_change_pw_OK_btn_clicked(GtkButton *button, gpointer user_data);
void on_pw_missmatch_OK_btn_clicked(GtkButton *button, gpointer user_data);
void on_invalid_pw_OK_btn_clicked(GtkButton *button, gpointer user_data);
void on_btn_lock_sys_clicked(GtkButton *button, gpointer user_data);
void on_pw_cancel_btn_clicked(GtkButton *button, gpointer user_data);
void on_pw_OK_btn_clicked(GtkButton *button, gpointer user_data);
void on_rb_auto_clicked(GtkButton *button, gpointer user_data);
void on_rb_user_clicked(GtkButton *button, gpointer user_data);
void on_calibrate_btn_clicked(GtkButton *button, gpointer user_data);

