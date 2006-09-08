/* [ tuner.h ] - Guitar Tuner 
 * Author: Matteo Bertozzi
 * ============================================================================
 * This file is part of Guitar Tuner.
 *
 * Guitar Tuner is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Guitar Tuner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __GUITAR_TUNER_H__
#define __GUITAR_TUNER_H__

#define NSAMPLES		(16384)	/* 16K -> 16384 */

typedef struct _note {
	char label[3];
	double freq; 
} Note;

typedef enum _tuner_adjust {
	TUNER_ADJUST_NONE,
	TUNER_ADJUST_DOWN,
	TUNER_ADJUST_UP
} GuitarTunerAdjust;

typedef struct _guitar_tuner {
	/* Device Descriptor */
	int fd;

	/* Channels: (Mono = 0, Stereo = 1) */
	int channels;

	/* AFMT_S8 is the Default 8bit Format
	 * You Must use Unsigned Char for the Data Read (in[N])
	 */
	int format;

	/* Sampleing Rate is Sampling Frequecy of the Microphone in Hz
	 * Lowering this Value will Lower f and improve your resolution.
	 * But it will alsa limit the highest frequency you can detect...
	 *    "You Must Sample at Twice The Highest Frequency...
	 *                      ...in order to prevent Aliasing."
	 */
	int sampling_frequency;

	/* Frequency Resolution (Smallest Frequency difference we can Detect)
	 *    f = sampling rate / numer of samples
	 * Increasing this value will Improve your Frequence Resolution.
	 * But if you make it too large, it takes longer to get each sample.
	 */	
	double frequency;

	/* cosine is just an array of cos values used to improve performance 
	 *     COSINE[i] = cos(2.0 * M_PI * (double) i / NSAMPLES);
	 */
	double COSINE[NSAMPLES];

	unsigned int max_index;
	double max;

	/* Frequence & Adjustament */
	double reference_freq;
	double actual_freq;
	const char *note;
	GuitarTunerAdjust adjust;
} GuitarTuner;

void tuner_init (GuitarTuner *tuner);
int tuner_sound_device_init (GuitarTuner *tuner, const char *device);
short int tuner_get_note (GuitarTuner *tuner);

#endif /* !__GUITAR_TUNER_H__ */
