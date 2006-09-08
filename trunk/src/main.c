/* [ main.c ] - Guitar Tuner
 * Author: Matteo Bertozzi
 * ============================================================================
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <pthread.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "tuner.h"
#include "gui.h"

void *tuner_thread_func (void *args) {
	GuitarTunerGUI *gui = (GuitarTunerGUI *) args;
	char buffer[256];

	while (1) {
		if (!tuner_get_note(&(gui->tuner))) {
			/* get GDK thread lock */
			gdk_threads_enter();

			gtk_image_set_from_file(GTK_IMAGE(gui->image_note), PIXMAPS_DIR "/Empty.png");
			gtk_image_set_from_file(GTK_IMAGE(gui->image_sharp), PIXMAPS_DIR "/NoSharp.png");
			gtk_label_set_markup(GTK_LABEL(gui->label_actual_freq), "---.-- Hz");
			gtk_label_set_markup(GTK_LABEL(gui->label_reference_freq), "---.-- Hz");

			/* release GDK thread lock */
			gdk_threads_leave();
			continue;
		}

		/* get GDK thread lock */
		gdk_threads_enter();		

		/* Reference Freq */
		snprintf(buffer, 256, "%3.2f Hz", gui->tuner.reference_freq);
		gtk_label_set_markup(GTK_LABEL(gui->label_reference_freq), buffer);

		/* Actual Freq */
		snprintf(buffer, 256, "%3.2f Hz", gui->tuner.actual_freq);
		gtk_label_set_markup(GTK_LABEL(gui->label_actual_freq), buffer);

		/* Have Sharp */
		if (gui->tuner.note[1] == '#') {			
			gtk_image_set_from_file(GTK_IMAGE(gui->image_sharp), PIXMAPS_DIR "/Sharp.png");
		} else {
			gtk_image_set_from_file(GTK_IMAGE(gui->image_sharp), PIXMAPS_DIR "/NoSharp.png");
		}

		/* Set Note Image */
		snprintf(buffer, 256, "%s/%c.png", PIXMAPS_DIR, gui->tuner.note[0]);
		gtk_image_set_from_file(GTK_IMAGE(gui->image_note), buffer);
	
		switch (gui->tuner.adjust) {
			case TUNER_ADJUST_NONE: {
				gtk_image_set_from_pixbuf(GTK_IMAGE(gui->image_adjustament), NULL);
				break;
			} case TUNER_ADJUST_DOWN: {
				gtk_image_set_from_file(GTK_IMAGE(gui->image_adjustament), 
										PIXMAPS_DIR "/Down.png");
				break;
			} case TUNER_ADJUST_UP: {
				gtk_image_set_from_file(GTK_IMAGE(gui->image_adjustament), 
										PIXMAPS_DIR "/Up.png");
				break;
			}
		}

		/* release GDK thread lock */
		gdk_threads_leave();
	}

	/* Never Reached */
	pthread_exit(NULL);
	return(NULL);
}

int main (int argc, char **argv) {
	const char *snd_device = NULL;
	pthread_t tuner_thread;
	GuitarTunerGUI gui;	

	if (argc > 1) {
		unsigned int i;

		for (i=1; i < argc; i++) {
			if (argv[i][0] != '-') continue;

			if (argv[i][1] == 'h') {
				printf("%s %s\n\n", GUITAR_TUNER_NAME, GUITAR_TUNER_VERSION);
				printf("Usage: %s [ option ]\n", argv[0]);
				printf("\n");
				printf("Option:\n");
				printf("   -h           Help. This Screen.\n");
				printf("   -v           Print Version and Exit.\n");
				printf("   -d <dev>     Select Sound (Input) Device\n");
				printf("\n");
				printf("Mail bug reports and suggestions to <theo.bertozzi@gmail.com>.\n");
				return(0);
			} else if (argv[i][1] == 'v') {
				printf("%s %s\n", GUITAR_TUNER_NAME, GUITAR_TUNER_VERSION);
				printf("Written by Matteo Bertozzi <theo.bertozzi@gmail.com>\n");
				return(0);
			} else if (argv[i][1] == 'd' && i++ < argc) {
				/* Device */
				snd_device = argv[i];
			} else {
				fprintf(stderr, "Invalid Syntax: -d <device name>\n");
				return(1);
			}
		}
	}

	/* Init Threads Support */
	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();

	/* Init Gtk Support */
	gtk_init(&argc, &argv);	

	/* Load Guitar Tuner GUI */
	guitar_tuner_gui_init(&gui);
	
	/* Init Tuner */
	tuner_init(&gui.tuner);
	if (tuner_sound_device_init(&gui.tuner, snd_device) < 0)
		return(1);

	/* Tuner Thread */
	pthread_create(&tuner_thread, NULL, 
			tuner_thread_func, &gui);

	/* End Of Gtk Loop and Threads... */
	gtk_main();
	gdk_threads_leave();
	return(0);
}

