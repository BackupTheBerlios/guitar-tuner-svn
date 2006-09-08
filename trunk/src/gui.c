/* [ gui.c ] - Guitar Tuner GUI
 * Author: Matteo Bertozzi
 * ============================================================================
 * This file is part of Guitar Tuner GUI.
 *
 * Guitar Tuner GUI is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Guitar Tuner GUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gtk/gtk.h>
#include <glib.h>
#include "gui.h"

static void on_destroy	(GtkWidget *widget, gpointer data);
static void on_about	(GtkWidget *widget, gpointer data);
static void guitar_tuner_gui_menu_init	(GuitarTunerGUI *gui);

static void on_destroy (GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

static void on_about (GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf_logo;
	GtkWidget *dialog;

	const gchar *authors[] = {
		"Matteo Bertozzi - theo.bertozzi@gmail.com", 
		"FFTW Programmers - http://www.fftw.org/",
		"OpenClipart Artists - http://www.openclipart.org",
		NULL
	};

	dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), GUITAR_TUNER_NAME);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), GUITAR_TUNER_VERSION);

	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
									"(C) Copyright 2006 Matteo Bertozzi.");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
									"http://guitar-tuner.berlios.de/");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);

	pixbuf_logo = gdk_pixbuf_new_from_file(PIXMAPS_DIR "/Logo.png", NULL);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf_logo);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void guitar_tuner_gui_menu_init (GuitarTunerGUI *gui) {
	GtkWidget *menu_item;
	GtkWidget *root_menu;
	GtkWidget *menu;

	/* Init Menu Bar */
	gui->menubar = gtk_menu_bar_new();
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->menubar, 0, 0);

	/* Init Menu FILE Items */
	menu = gtk_menu_new();

#if 0
	menu_item = gtk_separator_menu_item_new();
	gtk_menu_append(GTK_MENU(menu), menu_item);
#endif

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, NULL);
	g_signal_connect_swapped(G_OBJECT(menu_item), "activate",
							 G_CALLBACK(on_destroy), NULL);
	gtk_menu_append(GTK_MENU(menu), menu_item);

	root_menu = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(root_menu), menu);
	
	gtk_menu_bar_append(GTK_MENU_BAR(gui->menubar), root_menu);	

	/* Init Menu HELP Items */
	menu = gtk_menu_new();

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	g_signal_connect_swapped(G_OBJECT(menu_item), "activate",
							 G_CALLBACK(on_about), NULL);
	gtk_menu_append(GTK_MENU(menu), menu_item);

	root_menu = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(root_menu), menu);
	
	gtk_menu_bar_append(GTK_MENU_BAR(gui->menubar), root_menu);
}

void guitar_tuner_gui_init (GuitarTunerGUI *gui) {
	GdkPixbuf *win_icon;

	/* Init Guitar Tuner Window */
	gui->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(gui->window), 400, 270);
	gtk_window_set_resizable(GTK_WINDOW(gui->window), FALSE);
	gtk_window_set_title(GTK_WINDOW(gui->window), GUITAR_TUNER_NAME);

	/* Window Events */
	g_signal_connect(G_OBJECT(gui->window), "destroy",
					 G_CALLBACK(on_destroy), NULL);

	/* Set Window Icon */
	win_icon = gdk_pixbuf_new_from_file(PIXMAPS_DIR "/Icon.png", NULL);
	gtk_window_set_default_icon(win_icon);

	/* Init Guitar Tuner FixedBox */
	gui->fixedbox = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(gui->window), gui->fixedbox);

	/* Set Window Skin (Window Background) */
	gui->image_skin = gtk_image_new_from_file(PIXMAPS_DIR "/Skin.png");
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->image_skin, 0, 20);

	/* Setup Image Note */
	gui->image_note = gtk_image_new_from_file(PIXMAPS_DIR "/Empty.png");
	/* gui->image_note = gtk_image_new_from_file("Pixmap/A.png"); */
	gtk_widget_set_size_request(gui->image_note, 100, 100);
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->image_note, 40, 45);

	/* Setup Image Sharp */
	gui->image_sharp = gtk_image_new_from_file(PIXMAPS_DIR "/NoSharp.png");
	/* gui->image_sharp = gtk_image_new_from_file("Pixmap/Sharp.png"); */
	gtk_widget_set_size_request(gui->image_sharp, 70, 100);
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->image_sharp, 140, 55);

	/* Setup Label Reference Freq */
	gui->label_reference_freq = gtk_label_new("---.-- Hz");
	gtk_label_set_use_markup(GTK_LABEL(gui->label_reference_freq), TRUE);
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->label_reference_freq, 247, 219);

	/* Setup Label Actual Freq */
	gui->label_actual_freq = gtk_label_new("---.-- Hz");
	gtk_label_set_use_markup(GTK_LABEL(gui->label_actual_freq), TRUE);
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->label_actual_freq, 247, 193);

	/* Setup Image Sharp */
	gui->image_adjustament = gtk_image_new_from_pixbuf(NULL);
	/* gui->image_sharp = gtk_image_new_from_file("Pixmap/Sharp.png"); */
	gtk_widget_set_size_request(gui->image_adjustament, 29, 21);
	gtk_fixed_put(GTK_FIXED(gui->fixedbox), gui->image_adjustament, 280, 75);

	/* Init Menu BAR */
	guitar_tuner_gui_menu_init(gui);

	gtk_widget_show_all(gui->window);
}
