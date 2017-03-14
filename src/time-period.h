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
 * \file time-period.h
 * \brief Header for the time periods used in Synema frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This header files describes the time periods used in Synema frames, and
 * the functions related to time periods manipulation
 */
#ifndef __TIME_PERIOD_H
#define __TIME_PERIOD_H
#ifdef __cplusplus
extern "C" {
#endif



/** \struct time_period_t
 * \brief Time period to use for the display of a frame
 *
 * Represents the time range to use for the frame (for instance, 1 day, 3 weeks)
 * It is made of two components: one sets the time unit to use, the other one sets
 * the value. For instance, the unit may be week and the value 3, this will be
 * a 3 weeks time period.
 */
typedef struct {
	enum {
		LIVE=0,
		SECOND,
		MINUTE,	
		HOUR,
		DAY,
		WEEK,
		MONTH,
		YEAR,
		LAST_TIME_PERIOD_T	// DO NOT USE - FOR LOOP PURPOSE ONLY
	} type;		/**< Unit of time to use for the time period (Live means real time) */
	int value;	/**< Amount of time units (ie. seconds/months...) for this time period */
} time_period_t;



/** \fn char *time_period_get_display_name_type_value (int, int, gboolean)
 * \brief Returns the display name for a given time period (passed as two ints)
 *
 * This function creates an human readable string for a given time period, for
 * display purpose. You must not forget to free this string when it is not needed
 * anymore. The difference with time_period_get_display_name is that you pass
 * the type and value directly instead of the time period. This avoids you to
 * create a time period when you already have the values as integer.
 * @param[in] type the type of the time period
 * @param[in] value the value of the type period
 * @param[in] prepend_val if FALSE, only return the time unit (eg. month, year)
 * @return the time period's display name, to free with g_free
 */
char *time_period_get_display_name_type_value (int, int, gboolean);



/** \fn char *time_period_get_display_name (time_period_t *, gboolean)
 * \brief Returns the display name for a given time period
 *
 * This function creates an human readable string for a given time period, for
 * display purpose. You must not forget to free this string when it is not needed
 * anymore.
 * @param[in] period the time period for which you want an human readable name
 * @param[in] prepend_val if FALSE, only return the time unit (eg. month, year)
 * @return the time period's display name, to free with g_free
 */
char *time_period_get_display_name (time_period_t *, gboolean);



/** \fn time_period_t *time_period_new (int, long)
 * \brief Allocates the memory to hold a time period and fills it with the parameters.
 *
 * This function creates a time period. You must free it with time_period_free
 * when you don't need it anymore.
 * @param[in] type the time unit of the time period (WEEK, MONTH, etc)
 * @param[in] val the number of time units for the time period
 * @return the time period
 */
time_period_t *time_period_new (int, long);



/** \fn void time_period_free (time_period_t *)
 * \brief Frees the memory allocated in a time_period_t struct
 *
 * This function frees a time period given in parameter
 * @param[out] t the time period to free
 */
void time_period_free (time_period_t *);



/** \fn void time_period_copy (time_period_t *, time_period_t *)
 * \brief Copies the data of a time period into another, already allocated
 *
 * This function copies the data of a time period (src) into another (dest). The
 * dest time period must already be allocated.
 * @param[in] src the time period to copy from
 * @param[out] dest the time_period_t that will be copied into
 */
void time_period_copy (time_period_t *, time_period_t *);



/** \fn int time_period_cmp (const time_period_t *, const time_period_t *)
 * \brief Compares two time periods
 *
 * \remark This function does not allow quantifying the differences between two
 * time periods.
 *
 * This function compares the type and value of two time periods and tells if they
 * are equal or different.
 * @param[in] t1 the first time_period_t structure
 * @param[in] t2 the second time_period_t structure
 * @return 0 if t1 and t2 are equal, another value otherwise
 */
int time_period_cmp (const time_period_t *, const time_period_t *);
#ifdef __cplusplus
}
#endif
#endif
