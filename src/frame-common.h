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
 * \file frame-common.h
 * \brief Header for the Synema frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file describes the frame objects used in the Synema
 * project, via the frame_t struct, that contains pointers to plugin
 * functions, and members for the display period and machine, size of the
 * drawing area, etc.
 */
#ifndef __FRAME_COMMON_H
#define __FRAME_COMMON_H
#ifdef __cplusplus
extern "C" {
#endif

#include <dlfcn.h>
#include <gtk/gtk.h>

#include "data-types.h"
#include "frame-signals.h"
#include "func-list.h"
#include "machine.h"
#include "time-period.h"

/** \privatesection */
struct __frame_table_t; // Defined in frame-table.h
/** \publicsection */



/** \enum frame_error_t
 * \brief Type of frame error that can occur
 *
 * This enum serves to list the types of error that can occur in a frame.
 */
typedef enum __frame_error_t {
	ERROR_NONE=0,
	ERROR_MACHINE,
	ERROR_PERIOD,
	ERROR_SIZE,
	ERROR_PLUGIN
} frame_error_t;


/** \struct frame_t
 * \brief Data structure for the Synema frames
 *
 * This is a frame in which data will be displayed about a monitoring tool. The
 * struct contains all the data necessary, including the drawing area where each
 * plugin developer will display it's data, widgets to switch machines and
 * display periods, etc.
 *
 * The struct also contains pointers to functions that must be written by each
 * plugin developer.
 */
typedef struct __frame_t {
	gchar			*type;				/**< \brief Type of the monitoring tool for this frame */


	size_ratio_t	size;				/**< \brief Size ratio to use for that frame */
	machine_t		*display_machine;	/**< \brief The machine for which data should be displayed */
	time_period_t	display_period;		/**< \brief Period for which the data should be shown */


	gdouble			avg_refresh_time;	/**< \brief Last calculated average refresh time for a frame */
	gdouble			avg_refresh_timeout;/**< \brief Time left until recalculating the average refresh time */
	struct __frame_table_t	*parent;	/**< \brief The frames table to which this frame is attached */
	gboolean		hidden;				/**< \brief Tells whether the frame is being displayed or is in an hidden frames stack */
	guint			id;					/**< \brief An unique identifier for the frame */
	frame_error_t	last_err;			/**< \brief The type of the last error that occurred */
	

	/** \privatesection */
	GtkBuilder		*builder;			/**< \brief GtkBuilder containing the interface */
	GObject			*signal_box;		/**< \brief A GObject with signals useful for multithread communication inside a frame */
	gboolean		compute;			/**< \brief The computing thread runs till this becomes FALSE */
	gboolean		computing_requested;/**< \brief Tells if computing was requested while it was already being performed */
	GThread			*computing_thread;	/**< \brief The thread used to compute reports */
	GMutex			*mutex;				/**< \brief A mutex preventing multiple computing functions */
	GCond			*cond;				/**< \brief A thread cond used to force recomputing */
	GtkWidget		*computing_spinner;	/**< \brief A spinner that indicates computing is going on */
	gboolean		show_spinner;		/**< \brief Tells whether the spinner should still be shown or not */
	guint			spinner_start_src;	/**< \brief Useful to remove a g_timeout_add call when destroying the frame */
	guint			spinner_stop_src;	/**< \brief Useful to remove a g_idle_add call when destroying the frame */
	GtkToolItem		*hidden_item;		/**< \brief If hidden, a pointer to the tool item representing the frame in the hidden frames bar */
	gint			left_slot;			/**< \brief In the parent table, left index of the topleft slot used by the frame */
	gint			top_slot;			/**< \brief In the parent table, top index of the topleft slot used by the frame */
	gint			restore_position;	/**< \brief Position in the table, valid only when restoring a frame from a file */
	/** \publicsection */


	void			*priv;				/**< \brief A pointer to data specific to a monitoring tool */


	/** \brief Called during frame init, returns a list of functions that you
	 * want to be available for frames of your plugin's type
	 * 
	 * \remark The functions will be displayed in the same order they're discovered
	 * in the list.
	 *
	 * \remark If you don't want a menu with custom functions, just return -1
	 *
	 * In this function, you should make a GList containing func_list_t structs
	 * that all contain a display name and a pointer to a function.
	 *
	 * These functions will be added in a frame-specific menu, in the order they
	 * are found.
	 *
	 * The calling function is in charge of freeing the list.
	 * @return a list of func_list_t that will be added to the frame's menu
	 */
	GList *(*build_func_list)();


	/** \brief Called during frame init, returns a list of time periods available
	 * for your plugin
	 * 
	 * \remark If you want the frame to begin with a particular time period, put it in
	 * the beginning of the list. The calling function may not respect your choice.
	 *
	 * \remark You should always include the live mode.
	 *
	 * In this function, you should create a GList of time_period_t structs
	 * that you want to be available for your frame. It will be automatically called
	 * during the initialisation of the frame. The list will be freed
	 * automatically by the calling function.
	 * @return a list of time_period_t structures
	 */
	GList *(*build_time_periods)();


	/** \brief Called automatically when the frame is initialised
	 * 
	 * You should initialise your private_data structure and do anything you 
	 * need to do right after frame initialisation in this function.
	 * @param[in,out] frame_t * the frame that has just been initialised
	 * @return 0 if initialisation went well, another value otherwise
	 */
	int (*init_private)(struct __frame_t *);



	/** \brief Called automatically after a frame has been restored
	 *
	 * \note This function is optional.
	 *
	 * In this function, you should load from the GKeyFile anything you saved in
	 * the save_private function. This is meant to allow you to restore the
	 * state your private data had when your frame was saved.
	 * @param[out] frame_t * the frame whose private data should be restored
	 * @param[in] GKeyFile * the file containing the data to restore
	 * @return 0 if all the data was properly restored, another value otherwise
	 */
	int (*restore_private)(struct __frame_t *, GKeyFile *);



	/** \brief Called automatically when a frame is saved
	 *
	 * \note This function is optional.
	 *
	 * In this function, you should save in a GKeyFile anything in the frame's
	 * private data you need to can restore the state of the frame later.
	 * @param[out] frame_t * the frame whose private data should be saved
	 * @param[in] GKeyFile * the file in which the data should be saved
	 * @return 0 if all the data was entirely saved, another value otherwise
	 */
	int (*save_private)(struct __frame_t *, GKeyFile *);



	/** \brief Called automatically before the frame is destroyed
	 *
	 * In this function, you should free all the data pointed by the priv member.
	 * Do not free anything else, it will be taken care of by the free function
	 * calling this one before destroying the frame.
	 *
	 * @param[out] frame_t * the frame for which private data should be freed
	 * @return 0 if destruction went well, another value otherwise
	 */
	int (*free_private)(struct __frame_t *);


	/** \brief Tells if the frame's images should be recomputed
	 * 
	 * \warning Your implementation of this function must be very fast.
	 *
	 * This function tells if reports changed and the ploticus-generated
	 * pictures need to be drawn again.
	 * @param[in] frame_t * the frame which you want to check
	 * @return 1 if refreshing is needed, 0 if not, a negative value if error
	 */
	int (*is_refreshing_needed)(struct __frame_t *);


	/** \brief Computes the new images with ploticus
	 *
	 * This function tells if reports changed and the ploticus-generated
	 * images need to be drawn again. You can do anything such as generating
	 * these images and computing textual reports in this function.
	 *
	 * You should only compute the images for the current set of machine and time
	 * period. However, you can compute them for several sizes.
	 *
	 * @param[in,out] frame_t * the frame for which data should be refreshed
	 * @return 0 if refreshing went well, another value otherwise
	 */
	int (*refresh_content)(struct __frame_t *);


	/** \brief Draws what needs to be displayed in a cairo surface
	 *
	 * \warning Resource hungry operations are strictly prohibited in this function.
	 * It can be called dozens times per second.
	 *
	 * This function is where you should draw lines, text and pictures to the
	 * screen. It contains a cairo surface clipped to the region where redrawing
	 * is needed. You should only do cairo and gdk drawing here and you should
	 * not compute images with ploticus.
	 * @param[in] frame_t * the frame which needs to be redrawn
	 * @param[out] cairo_t * the cairo surface in which you should draw
	 * @return 0 if drawing went well, another value otherwise
	 */
	int (*draw_area)(struct __frame_t *, cairo_t *);


	/** \brief Returns the display name to use for the frame
	 *
	 * This function returns a static string that can be used as a display name
	 * for the frame in the main GUI.
	 * @param[in] frame_t * the frame whose name is wanted
	 * @return the frame's name
	 */
	char *(*get_display_name)(struct __frame_t *);


	/** \brief Reflets a frame size change in the plugin's internal data
	 *
	 * \warning It's better not to do anything heavy in this function as it can
	 * be called several times in a raw.
	 *
	 * This function may or may not be needed depending on your implementation.
	 * It's the place where you should change your internal data to make sure
	 * the drawing function now draws on a surface matching the new size of the
	 * drawing area.
	 * @param[in,out] frame_t * the frame whose size changed
	 * @return -1 if an error occurred, 0 otherwise
	 */
	int (*frame_size_changed)(struct __frame_t *);


	/** \brief Reflects the display machine change in the plugin's internal data
	 *
	 * \warning It's better not to do anything heavy in this function as it can
	 * be called several times in a raw.
	 * 
	 * In this function, you should make sure your internal data is in sync with
	 * the new machine, so that the data computed by ploticus matches this
	 * machine.
	 * @param[in,out] frame_t * the frame whose machine changed
	 * @return -1 if an error occurred, 0 otherwise
	 */
	int (*display_machine_changed)(struct __frame_t *);


	/** \brief Reflects the time period change in the plugin's internal data
	 *
	 * \warning It's better not to do anything heavy in this function as it can
	 * be called several times in a raw.
	 *	 
	 * In this function, you should make sure your internal data is in sync with
	 * the new time period, so that the data computed by ploticus matches this
	 * time period.
	 * @param[in,out] frame_t * the frame whose time period changed
	 * @return -1 if an error occurred, 0 otherwise
	 */
	int (*time_period_changed)(struct __frame_t *);


	/** \brief Handles mouse button press events
	 *
	 * \note This function is optional.
	 *
	 * You should use this function to handle mouse button press events. The 
	 * GdkEvent parameter will give you the coordinates, the mouse button 
	 * pressed, and it will tell if it is a single, double or triple click.
	 *
	 * You can then use this information to simulate a button click if the
	 * coordinates are in a rectangle drawn with cairo.
	 *
	 * \remark Coordinates are reachable with event->x and event->y which are
	 * of type double.
	 * \remark event->button will be 1 for left buttons, 2 for middle buttons,
	 * and 3 for right buttons.
	 * \remark event->type will be GDK_2BUTTON_PRESS for a double click, and
	 * GDK_3BUTTON_PRESS for a triple click. Note that there will also be 2
	 * (resp. 3) GDK_BUTTON_PRESS sent at the same time.
	 * @param[out] frame_t * the frame on which the event occurred
	 * @param[in] GdkEventButton * the data about the button press event
	 * @return 0 if no error occurrs, another value otherwise
	 */
	int (*button_press_handler)(struct __frame_t *, GdkEventButton *);


	/** \brief Handles mouse button release events
	 *
	 * \note This function is optional.
	 *
	 * You should use this function to handle mouse button release events. The 
	 * GdkEvent parameter will give you the coordinates, the mouse button 
	 * releaseed, and it will tell if it is a single, double or triple click.
	 *
	 * You can then use this information to simulate a button click if the
	 * coordinates are in a rectangle drawn with cairo.
	 *
	 * \remark Coordinates are reachable with event->x and event->y which are
	 * of type double.
	 * \remark event->button will be 1 for left buttons, 2 for middle buttons,
	 * and 3 for right buttons.
	 * \remark It's not possible to know if a button release is a single or
	 * double click.
	 * @param[out] frame_t * the frame on which the event occurred
	 * @param[in] GdkEventButton * the data about the button release event
	 * @return 0 if no error occurrs, another value otherwise
	 */
	int (*button_release_handler)(struct __frame_t *, GdkEventButton *);
} frame_t;


#ifdef __cplusplus
}
#endif
#endif
