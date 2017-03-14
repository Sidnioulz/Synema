/* Copyright 2009 (c) ENSI de Bourges
 * 		88 boulevard Lahitolle, 18020 Bourges Cedex, France
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file log-player-dbus.c
 * \brief Source code for our log player DBus client
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the source code of the log player's DBus client functions.
 */
#include <gtk/gtk.h>

#include "application-data.h"
#include "constants.h"
#include "log-player-dbus.h"
#include "log-player.h"
#include "log-player-dbus-marshal.h"



void player_dbus_playback_ended_callback (DBusGProxy *proxy, gpointer user_data)
{
	synema_instance_t	*inst	= synema_instance ();
	g_return_if_fail (proxy != NULL);
	player_on_stop_button_pressed (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_stop")), GINT_TO_POINTER (0));
} 



void player_dbus_date_being_played_callback (DBusGProxy *proxy, guint64 time, gpointer user_data)
{
	synema_instance_t	*inst	= synema_instance ();
	g_return_if_fail (proxy != NULL);
	inst->player_curr_time = time;
	player_set_status_label (time, "Playing logs from");
}



gboolean player_dbus_try_ping (gboolean set_values)
{
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	gboolean		pong		= FALSE;
	GError			*err		= NULL;
	GtkWidget		*status		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_align_status");
	GtkWidget		*start		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_align_start");
	GtkWidget		*stopafter	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_align_stopafter");
	GtkWidget		*speed		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_align_speed");
	GtkWidget		*errlabel	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_align_errlabel");

	if (!fr_ensi_bourges_synema_log_player_ping (inst->dbus_proxy, &pong, &err)) {
		g_warning ("player_dbus_try_ping: Ping failed (%s)", err->message);
		g_clear_error (&err);
		pong = FALSE;
	}

	if (pong) {
		gtk_widget_hide (errlabel);

		gtk_widget_show (status);
		gtk_widget_show (start);
		gtk_widget_show (stopafter);
		gtk_widget_show (speed);

		if (set_values)
			player_update_values_from_dbus ();
	} else {
		gtk_widget_show (errlabel);

		gtk_widget_hide (status);
		gtk_widget_hide (start);
		gtk_widget_hide (stopafter);
		gtk_widget_hide (speed);

		player_on_stop_button_pressed (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_stop")), GINT_TO_POINTER (0));
	}

	return pong;
}



gboolean player_dbus_set_speed (double speed)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_set_speed (inst->dbus_proxy, speed, &err)) {
		g_warning ("player_dbus_set_speed: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_get_speed (double *speed)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_get_speed (inst->dbus_proxy, speed, &err)) {
		g_warning ("player_dbus_get_speed: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_get_start_date (guint64 *date)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_get_start_date (inst->dbus_proxy, date, &err)) {
		g_warning ("player_dbus_get_start_date: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_get_duration (guint64 *duration)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_get_duration (inst->dbus_proxy, duration, &err)) {
		g_warning ("player_dbus_get_duration: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_get_state (gboolean *started, gboolean *paused)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_get_state (inst->dbus_proxy, started, paused, &err)) {
		g_warning ("player_dbus_get_state: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_stop ()
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_stop (inst->dbus_proxy, &err)) {
		g_warning ("player_dbus_stop: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_set_paused (gboolean paused)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_set_paused (inst->dbus_proxy, paused, &err)) {
		g_warning ("player_dbus_set_paused: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



gboolean player_dbus_start (guint64 start_date, guint64 duration)
{
	GError			*err	= NULL;
	synema_instance_t	*inst	= synema_instance ();
	g_return_val_if_fail (inst->dbus_connection != NULL, FALSE);

	if (!fr_ensi_bourges_synema_log_player_start (inst->dbus_proxy, start_date, duration, &err)) {
		g_warning ("player_dbus_start: DBus call failed (%s)", err->message);
		g_clear_error (&err);
		player_dbus_try_ping (FALSE);
		return FALSE;
	}

	return TRUE;
}



static void player_dbus_on_lost_proxy (DBusGConnection **connection)
{
	g_critical ("The DBus proxy has been lost. Unreferencing the DBus connection.");
	dbus_g_connection_unref (*connection);
	*connection = NULL;
}



gboolean player_dbus_connect (DBusGConnection **connection, DBusGProxy **proxy)
{
	GError	*err	= NULL;

	*connection = dbus_g_bus_get (DBUS_BUS_SESSION, &err);
	if (err) {
		g_warning ("player_dbus_connect: Could not get the DBus session bus (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}

	*proxy = dbus_g_proxy_new_for_name (*connection, PLAYER_SERVICE_NAME, PLAYER_PATH, PLAYER_INTERFACE_NAME);
	if (*proxy == NULL) {
		g_warning ("player_dbus_connect: Failed to create a DBus proxy for the log player");
		return FALSE;
	}

	g_signal_connect_swapped (*proxy, "destroy", G_CALLBACK (player_dbus_on_lost_proxy), connection);

	dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__UINT64, G_TYPE_NONE, G_TYPE_UINT64, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (*proxy, "DateBeingPlayed", G_TYPE_UINT64, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (*proxy,
						"DateBeingPlayed",
						G_CALLBACK (player_dbus_date_being_played_callback),
						NULL,
						NULL);
						
	dbus_g_object_register_marshaller (g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, G_TYPE_INVALID);
	dbus_g_proxy_add_signal (*proxy, "PlaybackEnded", G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (*proxy,
						"PlaybackEnded",
						G_CALLBACK (player_dbus_playback_ended_callback),
						NULL,
						NULL);
	return TRUE;
}

