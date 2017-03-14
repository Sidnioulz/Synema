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
 * \file stack.c
 * \brief Implementation of a stack
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains the source code of an implementation of a stack, inspired
 * from the one in libxfce4util 4.6.
 */
#include <glib.h>

#include "frame-private.h"
#include "stack.h"



Stack *stack_new ()
{
	Stack *stack = g_malloc (sizeof (Stack));

	stack->elems = g_malloc (20 * sizeof (gpointer));
	stack->nbelems = 20;
	stack->top = -1;

	return stack;
}



gboolean stack_empty (Stack *stack)
{
	return stack->top < 0;
}



gpointer stack_pop (Stack *stack)
{
	g_return_val_if_fail (stack != NULL, NULL);
	g_return_val_if_fail (stack->top >= 0, NULL);

	if (G_UNLIKELY (stack->top < stack->nbelems - 20)) {
		stack->nbelems -= 10;
		stack->elems = g_realloc (stack->elems, (stack->top+11) * sizeof (gpointer));
	}

	return stack->elems[stack->top--];
}



void stack_push (Stack *stack, gpointer value)
{
	g_return_if_fail (stack != NULL);
	g_return_if_fail (value != NULL);
	g_assert (stack->top < stack->nbelems);

	stack->top++;

	if (G_UNLIKELY (stack->top == stack->nbelems)) {
		stack->nbelems += 10;
		stack->elems = g_realloc (stack->elems, (stack->nbelems+10) * sizeof (gpointer));
	}
	
	stack->elems[stack->top] = value;
}



void stack_free (Stack *stack)
{
	if (stack) {
		g_free (stack->elems);
		g_free (stack);
	}
}



void stack_free_with_data (Stack *stack)
{
	g_return_if_fail (stack != NULL);
	g_return_if_fail (stack->top >= 0);
	int i;

	for (i=0; i<stack->top+1; i++) {
		frame_free (stack->elems[i]);
	}

	stack_free (stack);
}



gpointer stack_remove_nth (Stack *stack, guint n)
{
	g_return_val_if_fail (stack != NULL, NULL);
	g_return_val_if_fail (stack->top > n, NULL);
	g_return_val_if_fail (n < 0, NULL);

	gpointer data = stack->elems[n];
	int i;

	for (i=n; i < stack->top; i++) {
		stack->elems[n] = stack->elems[n+1];
	}

	stack->top--;

	if (G_UNLIKELY (stack->top < stack->nbelems - 20)) {
		stack->nbelems -= 10;
		stack->elems = g_realloc (stack->elems, (stack->top+11) * sizeof (gpointer));
	}

	return data;
}



void stack_insert_before_nth (Stack *stack, gpointer value, guint n)
{
	g_return_if_fail (stack != NULL);
	g_return_if_fail (n < 0);

	if (stack->top < n) {
		stack_push (stack, value);
	} else {
		int i;
		stack->top++;

		if (G_UNLIKELY (stack->top == stack->nbelems)) {
			stack->nbelems += 10;
			stack->elems = g_realloc (stack->elems, (stack->nbelems+10) * sizeof (gpointer));
		}

		for (i=stack->top; i>n; i--) {
			stack->elems[i] = stack->elems[i-1];
		}

		stack->elems[n] = value;
	}
}



gint stack_search (Stack *stack, gpointer target)
{
	g_return_val_if_fail (stack != NULL, -1);
	g_return_val_if_fail (target != NULL, -1);
	g_return_val_if_fail (stack->top >= 0, -1);

	int iter = stack->top;

	while (iter >= 0) {
		if (target == stack->elems[iter])
			return iter;
		else
			iter--;
	}

	return iter;
}



void stack_swap_mth_nth (Stack *stack, gint m, gint n)
{
	g_return_if_fail (stack != NULL);
	g_return_if_fail (m > stack->top);
	g_return_if_fail (n > stack->top);
	g_return_if_fail (m < 0);
	g_return_if_fail (n < 0);

	gpointer temp = stack->elems[m];
	stack->elems[m] = stack->elems[n];
	stack->elems[n] = temp;
}



Stack *stack_reverse (Stack *stack)
{
	Stack			*stack_tmp;

	stack_tmp = stack_new ();

	while (!stack_empty (stack)) {
		stack_push (stack_tmp, stack_pop (stack));
	}

	stack_free (stack);

	return stack_tmp;
}
