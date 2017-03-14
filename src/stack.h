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
 * \file stack.h
 * \brief Header for the stack implementation of Synema
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains function headers for Synema's frame stacks.
 */
#ifndef __STACK_H
#define __STACK_H
#ifdef __cplusplus
extern "C" {
#endif
#include <glib.h>



typedef struct __Stack {
	gpointer	*elems;
	gint		nbelems;
	gint		top;
} Stack;



Stack *stack_new ();
gboolean stack_empty (Stack *stack);
gpointer stack_pop (Stack *stack);
void stack_push (Stack *stack, gpointer value);
void stack_free (Stack *stack);
void stack_free_with_data (Stack *stack);
gpointer stack_remove_nth (Stack *stack, guint n);
void stack_insert_before_nth (Stack *stack, gpointer value, guint n);
gint stack_search (Stack *stack, gpointer target);
void stack_swap_mth_nth (Stack *stack, gint m, gint n);
Stack *stack_reverse (Stack *stack);
#ifdef __cplusplus
}
#endif
#endif
