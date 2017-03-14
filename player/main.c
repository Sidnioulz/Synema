#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <glib.h>

#include "dbus-consts.h"
#include "server.h"



int main (int argc, char **argv)
{
	DBusGConnection		*connection		= NULL;
	DBusGProxy			*proxy			= NULL;
	GError				*err 			= NULL;
	GMainLoop			*loop			= NULL;
	GObject				*LSPlayer_obj	= NULL;
	guint32				request_name_ret;

	g_type_init ();
	loop = g_main_loop_new (NULL, FALSE);

	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &err);
	if (err) {
		g_error ("Failed to open a DBus connection to the session bus (%s)", err->message);
		g_clear_error (&err);
	}

	LSPlayer_obj = g_object_new (LSPLAYER_OBJ_TYPE, NULL);
	dbus_g_connection_register_g_object (connection, LSPLAYER_PATH, LSPlayer_obj);

	proxy = dbus_g_proxy_new_for_name (connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);
	org_freedesktop_DBus_request_name (proxy, LSPLAYER_SERVICE_NAME, 0, &request_name_ret, &err);
	if (err) {
		g_error ("Failed to request the player service name to DBus (%s)", err->message);
		g_clear_error (&err);
	}
	if (request_name_ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got incorrect result code %u from requesting the player service name to DBus", request_name_ret);
	}

	g_debug ("The player is bound to the DBus service name '%s'", LSPLAYER_SERVICE_NAME);
	g_main_loop_run (loop);

	//TODO free all unfreed data
	return 0;
}
