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
 * \file log-player.c
 * \brief Source code for functions related to the log player's GUI
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the declaration of functions used by the log player's GUI.
 */
#include <gtk/gtk.h>
#include <time.h>

#include "application-data.h"
#include "log-player.h"
#include "log-player-dbus.h"
#include "time-period.h"



void player_set_status_label (guint64 timestamp, const gchar *prefix)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*buffer			= g_malloc (sizeof (char) * PLAYER_DATE_BUTTON_BUFFER_LEN);
	gchar				*finalbuffer	= NULL;
	GDate				*date			= g_date_new ();
	gint				hours,mins;
	GTimeVal			timeval			= {timestamp, 0};
	GtkWidget			*label			= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_status");
	guint64				tmptime			= 0;

	g_date_set_time_val (date, &timeval);
	g_date_strftime (buffer, PLAYER_DATE_BUTTON_BUFFER_LEN, "%x", date);

	tmptime = player_date_to_timestamp (date, 0, 0);
	tmptime = timestamp - tmptime;
	hours = tmptime / 3600;
	mins = (tmptime - 3600*hours) / 60;

	finalbuffer = g_strdup_printf ("%s\n%s at %02d:%02d", prefix, buffer, hours, mins);
	gtk_label_set_text (GTK_LABEL (label), finalbuffer);

	g_free (buffer);
	g_free (finalbuffer);
	g_date_free (date);
}



static gboolean date_button_get_date (GtkButton *button, GDate *date)
{
	g_date_set_parse (date, gtk_button_get_label (button));
	//TODO if g_date_valid fails, try to use strptime on systems where it's available
	return g_date_valid (date);
}



static gint date_button_set_label (GtkButton *button, GDate *date)
{
	gchar	*buffer	= g_malloc (sizeof (char) * PLAYER_DATE_BUTTON_BUFFER_LEN);

	if (g_date_strftime (buffer, PLAYER_DATE_BUTTON_BUFFER_LEN, PLAYER_DATE_BUTTON_STRFTIME_FORMAT, date) == 0) {
		g_free (buffer);
		return -1;
	}
	gtk_button_set_label (button, buffer);

	g_free (buffer);
	return 0;
}



static int player_stop_after_combo_get_type ()
{
	synema_instance_t	*inst	= synema_instance ();
	gint				type	= 0;
	GtkComboBox			*box	= GTK_COMBO_BOX (gtk_builder_get_object (inst->builder, "player_combo_playtime"));
	GtkTreeIter			iter;

	if (gtk_combo_box_get_active_iter (box, &iter)) {
		gtk_tree_model_get (gtk_combo_box_get_model (box), &iter, 1, &type, -1);
		return type;
	} else {
		return 0;
	}
}

guint64 player_date_to_timestamp (GDate *date, guint hours, guint mins)
{
	struct tm	tmbuffer	= {0};

	g_date_to_struct_tm (date, &tmbuffer);
	tmbuffer.tm_hour = hours;
	tmbuffer.tm_min = mins;
	tmbuffer.tm_sec = 0;

	return mktime (&tmbuffer);
}



static void player_on_play_button_pressed (GtkButton *button, gpointer user_data)
{
	synema_instance_t	*inst		= synema_instance ();
	gboolean			change_gui	= TRUE;

	/* We are currently stopped and start the player */
	if (inst->player_status == PLAYER_STOPPED) {
		GDate		*date		= g_date_new ();
		guint64		start_time	= 0;
		guint64		duration	= 0;

		if (GPOINTER_TO_INT (user_data)) {
			if (date_button_get_date (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_date")), date)) {
				start_time = player_date_to_timestamp (date,
								gtk_spin_button_get_value (GTK_SPIN_BUTTON (gtk_builder_get_object (inst->builder, "player_spinbutton_hour"))),
								gtk_spin_button_get_value (GTK_SPIN_BUTTON (gtk_builder_get_object (inst->builder, "player_spinbutton_min"))));

				if (start_time == -1) {
					g_warning ("player_on_play_button_pressed: Could not calculate a timestamp from the start date");
					g_date_free (date);
					return;
				}

				duration = gtk_spin_button_get_value (GTK_SPIN_BUTTON (gtk_builder_get_object (inst->builder, "player_spinbutton_playtime")));
				switch (player_stop_after_combo_get_type ()) {
					case WEEK:
						duration *=7; // In a week there are 7 days
					case DAY:
						duration *=24; // 24 hours
					case HOUR:
						duration *=60; // 60 mins
					case MINUTE:
						duration *=60; // 60 seconds
					case SECOND:
						// Now duration represents the number of seconds to let elapse
						break;
					
					// Particular cases: the number of seconds may differ depending on the start date
					case YEAR:
						duration *=12; // 12 months in a year, so now duration represents a number of months to add
					case MONTH:
						g_date_add_months (date, duration);
						duration = player_date_to_timestamp (date,
								gtk_spin_button_get_value (GTK_SPIN_BUTTON (gtk_builder_get_object (inst->builder, "player_spinbutton_hour"))),
								gtk_spin_button_get_value (GTK_SPIN_BUTTON (gtk_builder_get_object (inst->builder, "player_spinbutton_min"))));
						
						duration -= start_time;
						break;
					case LIVE:
					case LAST_TIME_PERIOD_T:
						duration = 0;
				}
			} else {
				g_warning ("player_on_play_button_pressed: Could not recognise the start date");
				g_date_free (date);
				return;
			}

			change_gui = player_dbus_start (start_time, duration);
			if (!change_gui)
				g_warning ("player_on_stop_button_pressed: Could not stop the player (DBus error)");
		}

		if (change_gui) {
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_date"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_hour"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_date_double_dot"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_min"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_playtime"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_combo_playtime"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_date"), FALSE);
			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_stopafter"), FALSE);

			gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_stop"), TRUE);
			gtk_image_set_from_stock (GTK_IMAGE (gtk_builder_get_object (inst->builder, "player_button_playpause_image")),
											GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
			inst->player_status = PLAYER_PLAYING;
			inst->player_curr_time = start_time;
			player_set_status_label (start_time, "Playing logs from");
		}
	}

	/* We are currently playing and pause the player */
	else if (inst->player_status == PLAYER_PLAYING) {
		if (!player_dbus_set_paused (TRUE)) {
			g_warning ("player_on_play_button_pressed: Could not pause the player (DBus error)");
		} else {
			gtk_image_set_from_stock (GTK_IMAGE (gtk_builder_get_object (inst->builder, "player_button_playpause_image")),
											GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
			
			inst->player_status = PLAYER_PAUSED;
			player_set_status_label (inst->player_curr_time, "Player paused on");
		}
	}

	/* We are currently paused and resume the player */
	else /* Paused */{
		if (!player_dbus_set_paused (FALSE)) {
			g_warning ("player_on_play_button_pressed: Could not resume the player (DBus error)");
		} else {
			gtk_image_set_from_stock (GTK_IMAGE (gtk_builder_get_object (inst->builder, "player_button_playpause_image")),
											GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);

			inst->player_status = PLAYER_PLAYING;
		}
	}
}



void player_on_stop_button_pressed (GtkButton *button, gpointer user_data)
{
	synema_instance_t	*inst		= synema_instance ();
	gboolean			change_gui	= TRUE;

	if (GPOINTER_TO_INT (user_data)) {
		change_gui = player_dbus_stop ();
		if (!change_gui)
			g_warning ("player_on_stop_button_pressed: Could not stop the player (DBus error)");
	}
	
	if (change_gui) {
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_date"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_hour"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_date_double_dot"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_min"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_playtime"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_combo_playtime"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_date"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_label_stopafter"), TRUE);
		
		gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
		gtk_image_set_from_stock (GTK_IMAGE (gtk_builder_get_object (inst->builder, "player_button_playpause_image")),
										GTK_STOCK_MEDIA_PLAY,  GTK_ICON_SIZE_BUTTON);
		gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (inst->builder, "player_status")), "Player not running");

		inst->player_status = PLAYER_STOPPED;
	}
}



static void player_show_date_calendar (GtkButton *button, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*dlg_objects[]	= {"calendar_dialog", NULL};
	gchar				*path			= NULL;
	GDate				*date			= g_date_new ();
	GError				*err			= NULL;
	gint				result;
	GtkWidget			*calendar		= NULL;
	GtkWidget			*dialog			= NULL;
	guint				year, month, day;


	// Add a new settings dialog to the builder
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, dlg_objects, &err);
	g_free (path);
	if (err) {
		g_warning ("player_show_date_calendar: %s", err->message);
		g_clear_error (&err);
		return;
	}


	// Setting the calendar's value to the current date in the button
	calendar = (GtkWidget *) gtk_builder_get_object (inst->builder, "calendar");
	if (date_button_get_date (button, date)) {
		gtk_calendar_select_month (GTK_CALENDAR (calendar), g_date_get_month (date) - 1, g_date_get_year (date));
		gtk_calendar_select_day (GTK_CALENDAR (calendar), g_date_get_day (date));
	}


	// Running the dialog
	dialog = (GtkWidget *) gtk_builder_get_object (inst->builder, "calendar_dialog");
	result = gtk_dialog_run (GTK_DIALOG (dialog));

	switch (result) {
		case GTK_RESPONSE_OK:
			gtk_calendar_get_date (GTK_CALENDAR (calendar), &year, &month, &day);
			g_date_set_dmy (date, day, month + 1, year);
			date_button_set_label (button, date);
			break;

		default:
			break;
	}

	g_date_free (date);
	gtk_widget_destroy (dialog);
}



static void player_on_spin_hour_wrapped (GtkSpinButton *button, gpointer user_data)
{
	GDate		*date		= g_date_new ();
	GtkButton	*datebutton	= GTK_BUTTON (user_data);

	if (date_button_get_date (datebutton, date)) {
		if (gtk_spin_button_get_value (button) == 0)
			g_date_add_days (date, 1);
		else
			g_date_subtract_days (date, 1);

		date_button_set_label (datebutton, date);
	}
}



/** \fn static void player_on_spin_minute_wrapped (GtkSpinButton *, gpointer)
 * \brief Called when the minutes spin button of the start date reaches 0 or 59
 *
 * This function is called when the minutes spin button's value is decrease when
 * it's worth 0 (resp. increased when worth 59). This causes the button's value
 * to be reset to 59 (resp. 0), and the hours spin button's value to be
 * decreased (resp. increased).
 * @param[in] button the minutes spin button of the log player's starting date
 * @param[out] user_data the hours spin button
 */
static void player_on_spin_minute_wrapped (GtkSpinButton *button, gpointer user_data)
{
	GtkSpinButton *hour = GTK_SPIN_BUTTON (user_data);

	if (gtk_spin_button_get_value (button) == 0)
		gtk_spin_button_spin (hour, GTK_SPIN_STEP_FORWARD, 1);
	else
		gtk_spin_button_spin (hour, GTK_SPIN_STEP_BACKWARD, 1);
}



/** \fn static void player_populate_stopafter_combo (GtkSpinButton *, gpointer)
 * \brief Fills a combo box with time period types
 *
 * This function retrieves the value of a GtkSpinButton meant to represent an
 * amount of time units, in order to determine the spelling (singular or plural)
 * to use to fill a combo box with time period types.
 * @param[in] button the spin button containing the number of time period units
 * @param[out] user_data the combo box to populate with time period types
 */
static void player_populate_stopafter_combo (GtkSpinButton *button, gpointer user_data)
{
	char			*name	= NULL;
	gint			active	= -1, i;
	GtkWidget		*combo	= user_data;
	GtkListStore	*store	= GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(combo)));
	GtkTreeIter		iter;

	active = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

	gtk_list_store_clear (store);
	for (i=LIVE+1; i<LAST_TIME_PERIOD_T; i++) {
		name = time_period_get_display_name_type_value (i, gtk_spin_button_get_value (button), FALSE);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, name, 1, i, -1);
		g_free (name);
	}

	gtk_combo_box_set_active (GTK_COMBO_BOX (combo), active);
}



/** \fn int player_get_scale_index_from_speed (double speed)
 * \brief Returns the index of a speed scale if it exists in the available ones
 *
 * This function, given a speed scale in number of seconds, tries to find it in
 * the available speed scale ratios (a static array identical to the one of
 * player_get_speed_scale). If it is found, the index of the speed ratio is
 * returned (so the speed scale can be set to that ratio). If not, then the
 * index for the normal ratio is returned.
 * @param[in] speed the speed ratio to use.
 * @return the index of the ratio if it is valid, the index of normal speed otherwise
 */
static int player_get_scale_index_from_speed (double speed)
{
	double speed_scale_ratio[] = {1.0/60, 1.0/30, .1, .25, .5, 1, 2, 4, 10, 30, 60,
									120, 300, 900, 3600, 86400, 604800};
	int i;

	for (i=0; i<17; i++) {
		if (speed == speed_scale_ratio[i])
			return i;
	}
	return PLAYER_SPEED_SCALE_INDEX_NORMAL;
}



/** \fn static double player_get_speed_scale (GtkRange *)
 * \brief Returns the speed scale to give to the log player
 *
 * This function, given a GtkScale representing the log player's speed scales,
 * returns the actual number of seconds to apply per second, so that it can be
 * sent to the log player.
 * @param[in] scale the scale whose speed to retrieve
 * @return the actual speed scale to apply
 */
static double player_get_speed_scale (GtkRange *scale)
{
	double speed_scale_ratio[] = {1.0/60, 1.0/30, .1, .25, .5, 1, 2, 4, 10, 30, 60,
									120, 300, 900, 3600, 86400, 604800};
	return speed_scale_ratio[(int) gtk_range_get_value (scale)];
}



/** \fn static void player_on_speed_scale_value_change (GtkScale *, gpointer)
 * \brief Updates the mark beneath the speed scale when it's value changes
 *
 * This function, called when the speed scale's value changes, updates the mark
 * beneath it indicating the current speed scale in an human readable way.
 * @param[in,out] scale the scale whose value changed
 * @param[in] user_data an int indicating if the app should tell the player to stop (if != 0)
 */
static void player_on_speed_scale_value_change (GtkScale *scale, gpointer user_data)
{
	//TRANSLATORS: for the rightmost mark, the spaces allow moving the label on the left, avoiding it being cut because of the lack of room. RTL languages may have to put the spaces on the left.
	const char *speed_scale_labels[] = {"1/60", "1/30", "1/10", "1/4", "1/2",
										"Normal", "2 sec", "4 sec", "10 sec", "30 sec",
										"1 min", "2 min", "5 min",
										"15 min", "1 hour", "1 day",
										"1 week    "};
	gboolean	change_mark	= TRUE;

	if (GPOINTER_TO_INT (user_data)) {
		change_mark = player_dbus_set_speed (player_get_speed_scale (GTK_RANGE (scale)));
		if (!change_mark)
			g_warning ("player_on_speed_scale_value_change: Could not update the speed scale (DBus error)");
	}
	
	if (change_mark) {
		gint value = gtk_range_get_value (GTK_RANGE (scale));
		gtk_scale_clear_marks (scale);
		gtk_scale_add_mark (scale, value, GTK_POS_BOTTOM, speed_scale_labels[value]);
	}
}



/** \fn static void player_decrement_speed_scale (GtkButton *, gpointer)
 * \brief Wrapper to decrease the player's speed scale
 *
 * This function is a wrapper that can be used on a GtkButton to decrement the
 * speed scale by 1.
 * @param[in] button a button that was pressed to trigger the event
 * @param[out] user_data a pointer to the speed scale
 */
static void player_decrement_speed_scale (GtkButton *button, gpointer user_data)
{
	gtk_range_set_value (GTK_RANGE (user_data), gtk_range_get_value (GTK_RANGE (user_data)) - 1);
}



/** \fn static void player_increment_speed_scale (GtkButton *, gpointer)
 * \brief Wrapper to increase the player's speed scale
 *
 * This function is a wrapper that can be used on a GtkButton to increment the
 * speed scale by 1.
 * @param[in] button a button that was pressed to trigger the event
 * @param[out] user_data a pointer to the speed scale
 */
static void player_increment_speed_scale (GtkButton *button, gpointer user_data)
{
	gtk_range_set_value (GTK_RANGE (user_data), gtk_range_get_value (GTK_RANGE (user_data)) + 1);
}



/** \fn static void player_on_lookup_button_clicked (GtkButton *, gpointer)
 * \brief Tries to contact the DBus server
 *
 * This function is a wrapper for player_dbus_try_ping which tries to join the
 * log player's DBus server. It is a handler for a button click.
 * @param[in] button an useless pointer to the button that was clicked
 * @param[in] user_data unused data
 */
static void player_on_lookup_button_clicked (GtkButton *button, gpointer user_data)
{
	player_dbus_try_ping (TRUE);
}



void player_update_values_from_dbus ()
{
	if (!player_dbus_try_ping (FALSE))
		return;

	synema_instance_t	*inst			= synema_instance ();
	gboolean			started			= FALSE;
	gboolean			paused			= FALSE;
	GDate				*date			= g_date_new ();
	gdouble				speed			= 0;
	GtkWidget			*datebutton		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_date");
	GtkWidget			*hourspin		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_hour");
	GtkWidget			*minspin		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_min");
	GtkWidget			*scale			= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_speed_scale");
	GtkWidget			*stopaftercombo	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_combo_playtime");
	guint64				duration;
	guint64				start_date;
	time_t				tt;
	struct tm			tm;


	player_dbus_get_state (&started, &paused);
	if (started) {
		// Emulate a press of the play button, without DBus call
		inst->player_status = PLAYER_STOPPED;
		player_on_play_button_pressed (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_playpause")), GINT_TO_POINTER (0));
		
		// Now status is playing, if we call the play button again it emulates a pause
		if (paused)
			player_on_play_button_pressed (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_playpause")), GINT_TO_POINTER (0));

			if (player_dbus_get_start_date (&start_date))
				tt = start_date;
			else
				tt = time (NULL);		
	} else {
		// Emulate a stop button press
		player_on_stop_button_pressed (GTK_BUTTON (gtk_builder_get_object (inst->builder, "player_button_stop")), GINT_TO_POINTER (0));

		tt = time (NULL);
	}
	

	// Set the start date widgets
	localtime_r (&tt, &tm);
	g_date_set_time_t (date, tt);
	date_button_set_label (GTK_BUTTON (datebutton), date);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (hourspin), tm.tm_hour);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (minspin), tm.tm_min);
    g_date_free (date);


	// Set the stop after widgets
	if (started && player_dbus_get_duration (&duration)) {
		/** TODO \todo
		 *  if duration % week_secs == 0
		 *  	is weeks
		 *  if duration % day_secs == 0
		 *  	is days
		 *  ...
		 *  if at end its neither week nor day nor hour nor min, then try to fetch the
		 
		 take date (store in a), add duration, get date (in b), compare timestamp (a) - timestamp (b) == duration, if == then is months/years, so calc difference
		 else is seconds
		 */
		g_warning ("player_update_values_from_dbus: Playback duration fetching not implemented"); 
	}
	gtk_combo_box_set_active (GTK_COMBO_BOX (stopaftercombo), 0);


	// Set the speed scale
	gtk_range_set_value (GTK_RANGE (scale),
							player_dbus_get_speed (&speed) ?
								player_get_scale_index_from_speed (speed) : 
								PLAYER_SPEED_SCALE_INDEX_NORMAL);
	player_on_speed_scale_value_change (GTK_SCALE (scale), GINT_TO_POINTER (0));
}



void player_setup_widgets ()
{
	synema_instance_t	*inst			= synema_instance ();
	GtkCellRenderer		*renderer 		= NULL;
	GtkWidget			*datebutton		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_date");
	GtkWidget			*hourspin		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_hour");
	GtkWidget			*minspin		= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_min");
	GtkWidget			*scale			= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_speed_scale");
	GtkWidget			*stopaftercombo	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_combo_playtime");
	GtkWidget			*stopafterspin	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_spinbutton_playtime");


	// Setting up the date button and time spin buttons
	g_signal_connect (datebutton, "clicked", G_CALLBACK (player_show_date_calendar), NULL);
	g_signal_connect (hourspin, "wrapped", G_CALLBACK (player_on_spin_hour_wrapped), datebutton);
	g_signal_connect (minspin, "wrapped", G_CALLBACK (player_on_spin_minute_wrapped), hourspin);


	// Connecting the "stop playing after" spin button to it's handler
	g_signal_connect (stopafterspin, "value-changed", G_CALLBACK (player_populate_stopafter_combo), stopaftercombo);


	// Setting up the "stop playing after" combo box
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (stopaftercombo), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (stopaftercombo), renderer, "text", 0, NULL);
	player_populate_stopafter_combo (GTK_SPIN_BUTTON (stopafterspin), stopaftercombo);


	// Setting up the speed scale
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_speed_minus"),
						"clicked", G_CALLBACK (player_decrement_speed_scale), scale);
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_speed_plus"),
						"clicked", G_CALLBACK (player_increment_speed_scale), scale);
	g_signal_connect (scale, "value-changed", G_CALLBACK (player_on_speed_scale_value_change), GINT_TO_POINTER (1));


	//  Setting the play and stop buttons on
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_playpause"),
						"clicked", G_CALLBACK (player_on_play_button_pressed), GINT_TO_POINTER (1));
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_button_stop"),
						"clicked", G_CALLBACK (player_on_stop_button_pressed), GINT_TO_POINTER (1));


	// Connecting the button to loop for the log player's DBus server
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "player_lookup_button"),
						"clicked", G_CALLBACK (player_on_lookup_button_clicked), NULL);


	// Set the value of the widgets and check for the player at the same time
	player_update_values_from_dbus ();
}
