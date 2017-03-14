#include <glib.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>

#include "server.h"
#include "dbus-methods.h"
#include "log-player-marshal.h"

G_DEFINE_TYPE(LSPlayerObject, object, G_TYPE_OBJECT);

gboolean _paused = FALSE;
gboolean _started = FALSE;
gboolean _neverends = FALSE;
gdouble _speed = 1;
guint64 _startdate = 0;
guint64 _duration = 0;
guint64 _curdate = 0;
guint64 _timeleft = 0;

/* FIXME currently if you add 0.1 second to _curdate it won't change since it's
 * an integer. Must add a gdouble _curdate_floating_part that will contain the
 * floating part of the current date to avoid immobilism on low speeds
 */

//Pour initialiser la classe (equiv. au constructeur en POO)
void object_class_init (LSPlayerObjectClass *klass)
{
	klass->signals[0] = g_signal_new ("date_being_played",
									G_OBJECT_CLASS_TYPE (klass),
									G_SIGNAL_RUN_LAST,
									0,
									NULL,
									NULL,
									g_cclosure_user_marshal_VOID__UINT64,
									G_TYPE_NONE,
									1,
									G_TYPE_UINT64);
									
	klass->signals[1] = g_signal_new ("playback_ended",
									G_OBJECT_CLASS_TYPE (klass),
									G_SIGNAL_RUN_LAST,
									0,
									NULL,
									NULL,
									g_cclosure_marshal_VOID__VOID,
									G_TYPE_NONE,
									0);

	dbus_g_object_type_install_info (LSPLAYER_OBJ_TYPE,
									&dbus_glib_object_object_info);
}

//Necessaire pour l'introspection du service
void object_init (LSPlayerObject *dummy)
{
}

gboolean send_pings (gpointer data)
{
	if ((_timeleft == 0) && !(_neverends)) {
		_started = FALSE;
		g_print ("Playback ends since the time left is 0\n");
		g_signal_emit_by_name (data, "playback_ended");
	} else if (!_paused) {
		_curdate += _speed;
		_timeleft -= MIN (_speed, _timeleft);

		g_print ("Emitting ping, date is now %llu\n", _curdate);
		g_signal_emit_by_name (data, "date_being_played", _curdate);
	}

	return _started;
}

gboolean start(LSPlayerObject *obj, guint64 start_date, guint64 duration, GError **err)
{
	_started = TRUE;
	_startdate = start_date;
	_curdate = start_date;
	_duration = duration;
	_timeleft = duration;
	_neverends = (duration == 0);
	_paused = FALSE;
	g_timeout_add (1000, send_pings, obj);
	g_print ("Playback starts with start_date %llu, for %llu seconds.\n", start_date, duration);
	return TRUE;
}

gboolean set_paused(LSPlayerObject *obj, gboolean paused, GError **err)
{
	_paused = paused;
	if (paused)
		g_print ("Playback is now paused.\n");
	else
		g_print ("Playback is now resumed.\n");
	return TRUE;
}

gboolean get_state(LSPlayerObject *obj, gboolean *started, gboolean *paused, GError **err)
{
	*started = _started;
	*paused = _paused;

	if (_started) {
		if (_paused)
			g_print ("The log player is paused.\n");
		else	
			g_print ("The log player is playing a file.\n");
	} else {
		g_print ("The log player is stopped.\n");
	}
	
	return TRUE;
}

gboolean get_current_date(LSPlayerObject *obj, guint64 *current_date, GError **err)
{
	g_print ("The current date is not implemented.\n");
	return TRUE;
}

gboolean get_time_left(LSPlayerObject *obj, guint64 *time_left, GError **err)
{
	g_print ("The time left is not implemented.\n");
	return TRUE;
}

gboolean stop(LSPlayerObject *obj, GError **err)
{
	_started = FALSE;
	g_source_remove_by_user_data (obj);
	g_print ("The log player has been stopped.\n");
	return TRUE;
}

gboolean get_speed(LSPlayerObject *obj, double *speed, GError **err)
{
	*speed = _speed;
	g_print ("Current speed is %f.\n", _speed);
	return TRUE;
}

gboolean get_start_date(LSPlayerObject *obj, double *date, GError **err)
{
	*date = _startdate;
	g_print ("Returning date %llu.\n", _startdate);
	return TRUE;
}

gboolean get_duration(LSPlayerObject *obj, double *duration, GError **err)
{
	*duration = _duration;
	g_print ("Returning date %llu.\n", _duration);
	return TRUE;
}

gboolean set_speed(LSPlayerObject *obj, double speed, GError **err)
{
	_speed = speed;
	g_print ("Speed has been set to %f.\n", _speed);
	return TRUE;
}

gboolean set_beginning_date(LSPlayerObject *obj, guint64 date, GError **err)
{
	g_print ("Beginning date should be set to %llu.\n", date);
	return TRUE;
}

gboolean set_length(LSPlayerObject *obj, guint64 len, GError **err)
{
	g_print ("Duration should be set to %llu.\n", len);
	return TRUE;
}

gboolean ping(LSPlayerObject *obj, gboolean *pong, GError **err)
{
	*pong = TRUE;//g_random_boolean ();
	g_print ("Pong is %i\n", *pong);
	return TRUE;
}
