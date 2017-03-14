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
 * \file time-period.c
 * \brief Source code for the time periods used in Synema frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains the source code of the time-period.h functions.
 */
#include <glib.h>

#include "time-period.h"



char *time_period_get_display_name_type_value (int type, int value, gboolean prepend_val)
{
	g_return_val_if_fail (type >= 0, NULL);
	g_return_val_if_fail (type < LAST_TIME_PERIOD_T, NULL);

	char *name = NULL;
	char *token = NULL;
	
	switch (type) {
		case LIVE:
			return g_strdup ("Live mode");
			
		case SECOND:
			if (value != 1)
				token = g_strdup ("seconds");
			else
				token = g_strdup ("second");
			break;
			
		case MINUTE:
			if (value != 1)
				token = g_strdup ("minutes");
			else
				token = g_strdup ("minute");
			break;
			
		case HOUR:
			if (value != 1)
				token = g_strdup ("hours");
			else
				token = g_strdup ("hour");
			break;
			
		case DAY:
			if (value != 1)
				token = g_strdup ("days");
			else
				token = g_strdup ("day");
			break;
			
		case WEEK:
			if (value != 1)
				token = g_strdup ("weeks");
			else
				token = g_strdup ("week");
			break;
			
		case MONTH:
			if (value != 1)
				token = g_strdup ("months");
			else
				token = g_strdup ("month");
			break;
			
		case YEAR:
			if (value != 1)
				token = g_strdup ("years");
			else
				token = g_strdup ("year");
			break;
		default:
			token = g_strdup ("unknown time unit");
	}

	if (prepend_val) {
		// TRANSLATORS: %s is the time unit (day, week, year, etc), and %d is the number of those time units, eg. 3 weeks, 5 days.
		name = g_strdup_printf ("%d %s", value, token);
		g_free (token);
		return name;
	} else {
		return token;
	}
}



char *time_period_get_display_name (time_period_t *period, gboolean prepend_val)
{
	return time_period_get_display_name_type_value (period->type, period->value, prepend_val);
}



time_period_t *time_period_new (int type, long val)
{
	time_period_t *t = g_malloc (sizeof (time_period_t));
	t->type = type;
	t->value = val;
	return t;
}



void time_period_free (time_period_t *t)
{
	g_free (t);
}



void time_period_copy (time_period_t *src, time_period_t *dest)
{
	dest->type = src->type;
	dest->value = src->value;
}



int time_period_cmp (const time_period_t *t1, const time_period_t *t2)
{
	if (t1 == NULL)
		return (t2 == NULL) ? 0 : -1;

	if (t2 == NULL)
		return 1;
	
	if (t1->type != t2->type)
		return t1->type - t2->type;
	else if (t1->type == LIVE)
		return 0;
	else if (t1->value != t2->value)
		return t1->value - t2->value;
	else
		return 0;
}
