#ifndef __SERVER_H__
#define __SERVER_H__

#include <glib-object.h>

GType object_get_type(void);

typedef enum {
    E_SIGNAL_DATE_BEING_PLAYED,
    E_SIGNAL_PLAYBACK_STOPPED,
    E_SIGNAL_COUNT
} LSPlayerSignalNumber ;

typedef struct{
   GObject parent;
}LSPlayerObject;

typedef struct{
   GObjectClass parent;
	guint signals[E_SIGNAL_COUNT];
}LSPlayerObjectClass;


#define LSPLAYER_OBJ_TYPE (object_get_type ())

gboolean start(LSPlayerObject*, guint64, guint64, GError**);
gboolean set_paused(LSPlayerObject*, gboolean, GError**);
gboolean get_state(LSPlayerObject*, gboolean*, gboolean*, GError**);
gboolean get_current_date(LSPlayerObject*, guint64*, GError**);
gboolean get_time_left(LSPlayerObject*, guint64*, GError**);
gboolean stop(LSPlayerObject*, GError**);
gboolean get_start_date(LSPlayerObject *obj, double *duration, GError **err);
gboolean get_duration(LSPlayerObject *obj, double *duration, GError **err);
gboolean get_speed(LSPlayerObject*, double*, GError**);
gboolean set_speed(LSPlayerObject*, double, GError**);
gboolean set_beginning_date(LSPlayerObject*, guint64, GError**);
gboolean set_length(LSPlayerObject*, guint64, GError**);
gboolean ping(LSPlayerObject*, gboolean*, GError**);

#endif
