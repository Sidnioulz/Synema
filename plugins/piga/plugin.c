#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"



char *get_display_name (frame_t *f)
{
	return "PIGA-IDS";
}



void all_parameters (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 1;

}



void int_all (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 2;

}

void data_int (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 3;

}

void tpe (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 4;

}

void do_int (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 5;

}

void non_int (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 6;

}

void fcc (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 7;

}

void data_conf (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 8;

}


GList *build_func_list ()
{
	GList *list = NULL;
	GList *children_list = NULL;
	GList *children_list2 = NULL;
	GList *children_list3 = NULL;

	func_list_add_new_func (&children_list2, "Data Confidentiality", data_conf);
	func_list_add_new_submenu (&list, "Confidentiality", children_list2);

	
	func_list_add_new_func (&list, "Integrity (all parameters)", int_all);

	
	func_list_add_new_func (&children_list, "Data Integrity", data_int);
	func_list_add_new_func (&children_list, "Trusted Path Execution", tpe);
	func_list_add_new_func (&children_list, "Domain Integrity", do_int);
	func_list_add_new_func (&children_list, "Non Interference", non_int);
	func_list_add_new_submenu (&list, "Integrity", children_list);


	func_list_add_new_func (&children_list3, "FCC", fcc);
	func_list_add_new_submenu (&list, "Adv", children_list3);

	func_list_add_new_separator (&list);
	func_list_add_new_func (&list, "All", all_parameters);

	return list;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4;

	p2 = time_period_new (WEEK, 1);
	list = g_list_prepend (list, p2);

	p3 = time_period_new (HOUR, 1);
	list = g_list_prepend (list, p3);

	p1 = time_period_new (MINUTE, 1);
	list = g_list_prepend (list, p1);
	
	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);

	return list;
}


int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = 1;
	//struct stat buf;
	//static time_t time_access=0;	
	char *path = frame_get_absolute_reports_path (f, "utype_enum", NULL);
	//stat(path, &buf);

	//if(buf.st_mtime > time_access)
	//{
	//	refreshing_needed=1;
	//	time_access = buf.st_mtime;
	//}	

	g_free (path);
//	return refreshing_needed;
	return 1;
}

int text_display(int ligne,char *dossier, char* reportspath1, frame_t *f) //weeks
{      
	char 		*path 			= frame_get_absolute_reports_path (f, "past_1_week/utype_enum", NULL);
	FILE 		*file 			= fopen(path, "r"); 

	if (file != NULL) // create new data text
	    {	
		char type[20]="", num[20]="";
		fseek(file, ligne , SEEK_SET);
		fscanf(file, "%s %s", type, num);
	        fclose(file);
		char * ploticus_file = frame_get_absolute_tmp_path(f,"pour_ploticus");
		FILE* fun=fopen(ploticus_file,"a");
		free(ploticus_file);		
			if(fun!=NULL)
			  {
	        	    fprintf(fun, "%s %s\n",type,num);
		            fclose(fun);
			  }
			else
			  {
			    printf("error\n");
			  }
	    }
	else
	    {
		printf("echec cuisant\n");
	    }
        return 0;
}

int text_display2(int ligne,char *dossier, char* reportspath1, frame_t *f) //hours
{     
	char 		*path 			= frame_get_absolute_reports_path (f, "past_1_hour/utype_enum", NULL);
	FILE 		*file 			= fopen(path, "r"); 
	 
	if (file != NULL) // create new data text
	    {	
		char type[20]="", num[20]="";
		fseek(file, ligne , SEEK_SET);
		fscanf(file, "%s %s", type, num);
	        fclose(file);
		char * ploticus_file = frame_get_absolute_tmp_path(f,"pour_ploticus");
		FILE* fun=fopen(ploticus_file,"a");
		free (ploticus_file);		
			if(fun!=NULL)
			  {
	        	    fprintf(fun, "%s %s\n",type,num);
		            fclose(fun);
			  }
			else
			  {
			    printf("error\n");
			  }
	    }
	else
	    {
		printf("echec cuisant\n");
	    }
    	return 0;
}

int text_display3(int ligne,char *dossier, char* reportspath1, frame_t *f) //minutes
{ 
	char 		*path 			= frame_get_absolute_reports_path (f, "past_1_min/utype_enum", NULL);
	FILE 		*file 			= fopen(path, "r"); 

	if (file != NULL) // create new data text
	    {	
		char type[20]="", num[20]="";
		fseek(file, ligne , SEEK_SET);
		fscanf(file, "%s %s", type, num);
	        fclose(file);
		char * ploticus_file = frame_get_absolute_tmp_path(f,"pour_ploticus");
		FILE* fun=fopen(ploticus_file,"a");
		free(ploticus_file);		
			if(fun!=NULL)
			  {
	        	    fprintf(fun, "%s %s\n",type,num);
		            fclose(fun);
			  }
			else
			  {
			    printf("error\n");
			  }
	    }
	else
	    {
		printf("echec cuisant\n");
	    }
    return 0;
}

int text_display4(int ligne,char *dossier, char* reportspath1, frame_t *f) //live
{      
	char 		*path 			= frame_get_absolute_reports_path (f, "past_10_sec/utype_enum", NULL);
	FILE 		*file 			= fopen(path, "r"); 

	if (file != NULL) // create new data text
	    {	
		char type[20]="", num[20]="";
		fseek(file, ligne , SEEK_SET);
		fscanf(file, "%s %s", type, num);
	        fclose(file);
		char * ploticus_file = frame_get_absolute_tmp_path(f,"pour_ploticus");
		FILE* fun=fopen(ploticus_file,"a");
		free (ploticus_file);		
			if(fun!=NULL)
			  {
	        	    fprintf(fun, "%s %s\n",type,num);
		            fclose(fun);
			  }
			else
			  {
			    printf("error\n");
			  }
	    }
	else
	    {
		printf("echec cuisant\n");
	    }
    return 0;
}


int refresh_content (frame_t *f,cairo_t *cr)
{
	char * ploticus_path = frame_get_absolute_tmp_path(f,"pour_ploticus");
	remove(ploticus_path);
	free(ploticus_path);
	data_t * data_plug = f->priv;
	char* tmppath =frame_get_absolute_tmp_path(f, "img_piga.svg");//path where the ploticus image is created
	char *command=NULL;
	char *reportspath = NULL;
	char *newpath=NULL;
	char *path=NULL;

	newpath = frame_get_absolute_tmp_path(f,"pour_ploticus");
	if (f->display_period.type == LIVE && f->display_period.value == 0)//LIVE MODE 
	{

		reportspath =frame_get_absolute_reports_path(f, "past_10_sec", NULL);	
		path=frame_get_absolute_reports_path(f, "past_10_sec/utype_enum",NULL);
		
		if (data_plug->entier==1)//Violations 
			command= g_strdup_printf("ploticus -prefab pie -svg -o %s data=%s  labels=1 values=2 ", tmppath, path);
		else if (data_plug->entier==2)
			command= g_strdup_printf("ploticus -prefab pie -svg -o %s data=%s labels=1 values=2 ", tmppath, path);
		else if (data_plug->entier==3)
			{text_display4(13,"utype_enum",reportspath, f);		
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==4)
			{text_display4(64,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==5)
			{text_display4(26,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==6)
			{text_display4(52,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==7)
			{text_display4(39,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==8)
			{text_display4(0,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3", newpath, tmppath);}
	
	}

	if (f->display_period.type == MINUTE && f->display_period.value == 1)//minute mode
	{
		reportspath =frame_get_absolute_reports_path(f, "past_1_min", NULL);	
		path=frame_get_absolute_reports_path(f, "past_1_min/utype_enum",NULL);
		
		if (data_plug->entier==1)
			command= g_strdup_printf("ploticus -prefab pie -svg data=%s -o %s labels=1 values=2 ", path, tmppath);
		else if (data_plug->entier==2)
			command= g_strdup_printf("ploticus -prefab pie -svg data=%s -o %s labels=1 values=2 ", path, tmppath);
		else if (data_plug->entier==3)
			{text_display3(13,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==4)
			{text_display3(64,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==5)
			{text_display3(26,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==6)
			{text_display3(52,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==7)
			{text_display3(39,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==8)
			{text_display3(0,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		
	}

	if (f->display_period.type == WEEK && f->display_period.value == 1)//last week
	{
		reportspath =frame_get_absolute_reports_path(f, "past_1_week",NULL);	
		path=frame_get_absolute_reports_path(f, "past_1_week/utype_enum",NULL);
		
		if (data_plug->entier==1)
			command= g_strdup_printf("ploticus -prefab pie -svg data=%s -o %s labels=1 values=2 ",path, tmppath);
		else if (data_plug->entier==2)
			command= g_strdup_printf("ploticus -prefab pie -svg data=%s -o %s labels=1 values=2 ",path, tmppath);
		else if (data_plug->entier==3)
			{text_display(13,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==4)
			{text_display(64,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==5)
			{text_display(26,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==6)
			{text_display(52,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==7)
			{text_display(39,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==8)
			{text_display(0,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}

	}

	if (f->display_period.type == HOUR && f->display_period.value == 1)//last hour
	{
		reportspath =frame_get_absolute_reports_path(f, "past_1_hour",NULL);	
		path=frame_get_absolute_reports_path(f, "past_1_hour/utype_enum",NULL);

		if (data_plug->entier==1)
			command= g_strdup_printf("ploticus -prefab pie -svg -o %s data=%s labels=1 values=2 ", tmppath, path);
		else if (data_plug->entier==2)
			command= g_strdup_printf("ploticus -prefab pie -svg -o %s data=%s labels=1 values=2 ", tmppath, path);
		else if (data_plug->entier==3)
			{text_display2(13,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==4)
			{text_display2(64,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==5)
			{text_display2(26,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==6)
			{text_display2(52,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==7)
			{text_display2(39,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}
		else if (data_plug->entier==8)
			{text_display2(0,"utype_enum",reportspath, f);			
			command= g_strdup_printf("ploticus -prefab vbars delim=" " -svg data=%s -o %s  x=1  y=2  vals=yes barwidth=0.3 ", newpath, tmppath);}

	}

	system(command);
	frame_request_redraw(f);
	g_free(command);
	g_free(reportspath);
	g_free(newpath);
	g_free(path);	
	g_free(tmppath);
	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	data_t *data = (data_t *) f->priv;
	data->entier = 1;
	frame_request_redraw (f);	
	return 0;
}


int free_private (frame_t *f)
{
	data_t *data = (data_t *) f->priv;
	free (data);

	return 0;
}

int restore_private(frame_t *f, GKeyFile *file)
{
	data_t 	*data = (data_t *) f->priv;
	GError 	*err = NULL;

	data->entier = g_key_file_get_integer (file, f->get_display_name (f), "Graphic", &err);
	if (err) {
		fprintf (stderr, "Could not load the graphic type (%s)\n", err->message);
		g_clear_error (&err);
	}

	return 0;
}

int save_private(frame_t *f, GKeyFile *file)
{
	data_t 		*data 		= (data_t *) f->priv;

	g_key_file_set_integer (file, f->get_display_name (f), "Graphic" ,data->entier);

	return 0;
}

int display_machine_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}



int frame_size_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}

char * piga_graphic_title (frame_t *f)
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*title		= NULL;

	if (data->entier == 1)
		title = g_strdup("All violations");
	else if (data->entier == 2)
		title = g_strdup("All integrity violations");
	else if (data->entier == 3)
		title = g_strdup("DATA integrity violations");
	else if (data->entier == 4)
		title = g_strdup("Trusted path violations");
	else if (data->entier == 5)
		title = g_strdup("Domain integrity violations");
	else if (data->entier == 6)
		title = g_strdup("Non violations");
	else if (data->entier == 7)
		title = g_strdup("FCC violations");
	else if (data->entier == 8)
		title = g_strdup("Data Conf");
	else 
		title = g_strdup("All violations");
	
	return title;
}



int draw_area (frame_t *f, cairo_t *cr)
{
	char 				*path		= NULL;
	char 				*title		= NULL;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	



	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);		
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.07);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);

	path = frame_get_absolute_tmp_path (f, "img_piga.svg");
	myhandle = rsvg_handle_new_from_file (path, &err);
	g_free (path);
	if (err) { 
		g_clear_error (&err);
	}
	else {
		cairo_save (cr); 
			cairo_scale (cr, frame_get_width (f)*0.51, frame_get_height (f)*0.68);			
			cairo_translate (cr, 0.50, 0.2);
		cairo_rotate (cr, M_PI);
		rsvg_handle_render_cairo (myhandle, cr);
		cairo_restore (cr);
		g_object_unref (myhandle);

		title = piga_graphic_title (f); 
		cairo_set_font_size (cr, frame_get_height (f)*0.05);
		cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);		
		cairo_show_text (cr, title);
		g_free (title);
	}

	return 0;
}


int button_press_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}

int button_release_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}
