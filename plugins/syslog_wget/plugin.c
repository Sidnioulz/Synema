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
 * \file plugin.c
 * \brief Fake Syslog (Wget) plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake Syslog (Wget) plugin for demonstration purpose.
 */
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"



char *get_display_name (frame_t *f)
{
	return "Syslog (Wget)";
}



void test_more_kitties (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;
	
	data->entier += 24;
	printf ("There now are %d kitties.\n", data->entier);
}



void test_kittycide (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;
	
	data->entier /= 4;
	printf ("A kittycide just happened! Only %d kitties survived...\n", data->entier);
}



GList *build_func_list ()
{
	GList *list = NULL;
	GList *children_list = NULL;

	func_list_add_new_func (&children_list, "Kitty genocide", test_kittycide);
	func_list_add_new_func (&children_list, "More kitties", test_more_kitties);
	func_list_add_new_submenu (&list, "Kitty stuff", children_list);
	func_list_add_new_separator (&list);
	func_list_add_new_func (&list, "The return of the kittycide", test_kittycide);

	return list;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4;
	
	p1 = time_period_new (WEEK, 2);
	list = g_list_prepend (list, p1);
	
	p2 = time_period_new (DAY, 2);
	list = g_list_prepend (list, p2);
	
	p3 = time_period_new (HOUR, 6);
	list = g_list_prepend (list, p3);
	
	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);
	
	return list;
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 5);
	char *path = frame_get_absolute_reports_path (f, "mon_rapport.txt", NULL);

	//You must now stat path to get it's mtime. Make sure to manage errors properly.

	g_free (path);
	return refreshing_needed;
}



int refresh_content (frame_t *f)
{
	// All the code should be embed in if (f->compute) blocks. This is to ensure
	// the function will quit as fast as possible if it's asked not to compute
	// anymore.
	
	if (f->compute) {
		sleep (1);
	}

	if (f->compute) {
		int x,i;
		for (x=i=0;i<4000000;i++) {
			x += i;
		}
	}
	
	if (f->compute) {
		printf ("The ploticus image has been recomputed, now we should redraw the drawing area.\n");
		frame_request_redraw (f);
	}

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
	printf ("Initialising my plugin's private data here.\n");
	
	data->chaine = strdup ("Hello kitty.\n");
	data->entier = 42;
	
	return 0;
}



int free_private (frame_t *f)
{
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
	printf ("Here I do not forget to free all the memory I allocated.\n");
	
	free (data->chaine);
	free (data);

	return 0;
}



int display_machine_changed (frame_t *f)
{
	printf ("Here, I must change my internal data to reflect the machine change.\n");
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
	printf ("Here, I must change my internal data to reflect the time period change.\n");
	frame_request_content_refresh (f);
	return 0;
}



int frame_size_changed (frame_t *f)
{
	printf ("Here, I must change my internal data to reflect the frame size change.\n");
	frame_request_content_refresh (f);
	return 0;
}



/* À voir
 * CAIRO
 * 	http://library.gnome.org/devel/cairo/stable/cairo-context.html
 * 	http://www.cairographics.org/manual/cairo-transformations.html
 *
 * IMAGES PNG AVEC CAIRO
 * 	http://library.gnome.org/devel/cairo/stable/cairo-image-surface.html
 *
 * IMAGES SVG AVEC CAIRO
 * 	http://library.gnome.org/devel/rsvg/stable/rsvg-Core-API.html
 * 	http://library.gnome.org/devel/rsvg/stable/rsvg-Cairo.html
 * 
 */
int draw_area (frame_t *f, cairo_t *cr)
{
	cairo_surface_t		*image		= NULL;
	char				*path		= NULL;
	double				tmp;
	GError				*err		= NULL;
	int					w,h;
	RsvgHandle			*myhandle	= NULL;


	/* Permet de dessiner sur la surface en intégralité: crée un rectangle de la taille
	de la zone de dessin sur le masque. */
	cairo_rectangle (cr, 0, 0, frame_get_width (f), frame_get_height (f));

	/* Crée un cercle avec un contour */
	cairo_new_sub_path (cr); cairo_arc_negative (cr, frame_get_width (f)*0.7, frame_get_height (f)*0.25, 40, 0, -2*M_PI);

	/* Remplit le fond en bleu, sans colorier l'intérieur du cercle. */
	cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);
	cairo_set_source_rgb (cr, 0, 0, 0.9);
	cairo_fill_preserve (cr);

	/* Dessine le contour du cercle */
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);


	/* Importe une image png d'un fichier. La fonction frame_get_absolute_data_path
	permet d'aller chercher l'image dans le dossier data du plugin. */
	path = frame_get_absolute_data_path (f, "a.png");
	image = cairo_image_surface_create_from_png (path);
	free (path);
	w = cairo_image_surface_get_width (image);
	h = cairo_image_surface_get_height (image);
	tmp = frame_get_height (f) / h;

	/* Permet de sauvegarder l'état de la surface avant de la modifier */
	cairo_save (cr);
	/* Redimensionnne la surface pour y coller un bout de tux */
	cairo_scale (cr, tmp, tmp);
	cairo_set_source_surface (cr, image, 0, 0);
	cairo_paint (cr);
	cairo_surface_destroy (image);

	/* Restaure la surface, sans supprimer l'image ajoutée entre temps */
	cairo_restore (cr);


	/* Une autre image, décalée à droite par rapport à la première */
	path = frame_get_absolute_data_path (f, "b.png");
	image = cairo_image_surface_create_from_png (path);
	free (path);
	h = cairo_image_surface_get_height (image);
	tmp = frame_get_height (f) / h;

	cairo_save (cr);
	cairo_scale (cr, tmp, tmp);
	cairo_set_source_surface (cr, image, w-1, 0);
	cairo_paint (cr);
	cairo_surface_destroy (image);
	cairo_restore (cr);


	/* Ajout d'un bout de text. Définir une taille de police avec frame_get_height (f)*0.08
	permet de rester indépendant de la résolution d'affichage. Merci d'avance d'utiliser
	la police sans exclusivement. */
 	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
 	cairo_set_font_size (cr, frame_get_height (f)*0.06);
 	cairo_set_source_rgb (cr, 1.0, 1.0, 0.0);
 	cairo_move_to (cr, frame_get_width (f)*0.7, frame_get_height (f)*0.92);

	/* Un test pour changer l'affichage pour chaque période de temps */
	if (f->display_period.type == LIVE)
	    cairo_show_text (cr, "Live mode");
	else if (f->display_period.type == SECOND)
	    cairo_show_text (cr, "Second");
	else if (f->display_period.type == MINUTE)
	    cairo_show_text (cr, "Minute");
	else if (f->display_period.type == HOUR)
	    cairo_show_text (cr, "Hour");
	else if (f->display_period.type == DAY)
	    cairo_show_text (cr, "Day");
	else if (f->display_period.type == WEEK)
	    cairo_show_text (cr, "Week");
	else if (f->display_period.type == MONTH)
	    cairo_show_text (cr, "Month");
	else if (f->display_period.type == YEAR)
	    cairo_show_text (cr, "Year");
	else
	    cairo_show_text (cr, "Unknown time period");

 	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
 	cairo_set_font_size (cr, frame_get_height (f)*0.06);
 	cairo_set_source_rgb (cr, 1.0, 1.0, 0.0); 	
 	cairo_move_to (cr, frame_get_width (f)*0.7, frame_get_height (f)*0.82);

	if (machine_is_all_machines (f->display_machine)) {
		cairo_show_text (cr, "Global stats");
	} else {
		cairo_show_text (cr, machine_get_display_name (f->display_machine));
	}


	/* Ajout d'un trait avec une terminaison ronde :
	On travaille sur le masque
	On déplace tout d'abord le "crayon" de cairo à 10% de la taille de l'image en haut à gauche
	On donne la destination du "crayon"
	On définis l'épaisseur du trait à 4 pixels
	On indique comment le trait se termine : ici en forme de demi cercle
	On "tamponne" le masque sur la surface, de sorte à former une ligne noire */  
 	cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.1);
 	cairo_set_source_rgb (cr, 0, 0, 0);
 	cairo_set_line_width (cr, 4);
	cairo_line_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.3);
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_stroke (cr);

	/* Importation d'une image SVG */
	path = frame_get_absolute_data_path (f, "test.svg");
	myhandle = rsvg_handle_new_from_file (path, &err);
	free (path);
	if (err) {
		g_warning ("%s", err->message);
		g_error_free (err);
	}
	else {
		cairo_save (cr);
		/* Modifie la taille de la surface de rendu */
		cairo_scale (cr, 2, 2);
		/* Déplace la surface */
		cairo_translate (cr, frame_get_width (f)*0.3, frame_get_height (f)*0.2);
		/* Imprime l'image */
		rsvg_handle_render_cairo (myhandle, cr);
		/* Libère la mémoire */
		rsvg_handle_free (myhandle);
		cairo_restore (cr);
	}

	return 0;
}
