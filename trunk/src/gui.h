/* [ gui.h ] - Guitar Tuner GUI
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

#ifndef __GUITAR_TUNER_GUI_H__
#define __GUITAR_TUNER_GUI_H__

#include <gtk/gtk.h>
#include <glib.h>
#include "tuner.h"

#define GUITAR_TUNER_NAME	"Guitar Tuner"
#define GUITAR_TUNER_VERSION	"0.1.2"

typedef struct _guitar_tuner_gui {
	/* Base Window */
	GtkWidget *image_skin;
	GtkWidget *fixedbox;
	GtkWidget *window;

	/* Menu Bar */
	GtkWidget *menubar;

	/* Note + Sharp Image */
	GtkWidget *image_note;
	GtkWidget *image_sharp;

	/* Frequence Label */
	GtkWidget *label_actual_freq;
	GtkWidget *label_reference_freq;

	/* Adjustament */
	GtkWidget *image_adjustament;

	GuitarTuner tuner;
} GuitarTunerGUI;

void guitar_tuner_gui_init (GuitarTunerGUI *gui);

#endif /* !__GUITAR_TUNER_GUI_H__ */
