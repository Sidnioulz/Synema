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
 * \file log-player-dbus.h
 * \brief Header for our log player DBus client
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the signature of the log player's DBus client functions.
 */
#ifndef __LOG_PLAYER_DBUS_H
#define __LOG_PLAYER_DBUS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <dbus/dbus-glib-bindings.h>
#include <glib.h>

#include "log-player-dbus-methods.h"



void player_dbus_playback_ended_callback (DBusGProxy *proxy, gpointer user_data);

void player_dbus_date_being_played_callback (DBusGProxy *, guint64, gpointer);

gboolean player_dbus_set_speed (double speed);

gboolean player_dbus_get_speed (double *speed);

gboolean player_dbus_get_start_date (guint64 *date);

gboolean player_dbus_get_duration (guint64 *duration);

gboolean player_dbus_get_state (gboolean *started, gboolean *paused);

gboolean player_dbus_stop ();

gboolean player_dbus_set_paused (gboolean paused);

gboolean player_dbus_start (guint64 start_date, guint64 duration);

gboolean player_dbus_try_ping (gboolean set_values);

gboolean player_dbus_connect (DBusGConnection **, DBusGProxy **);
#ifdef __cplusplus
}
#endif
#endif
