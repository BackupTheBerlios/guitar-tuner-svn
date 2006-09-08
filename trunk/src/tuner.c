/* [ tuner.c ] - Guitar Tuner 
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

/* Standard Headers */
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

/* Fourier Transform Header */
#include <fftw3.h>

#include "tuner.h"

const char *DEFAULT_DEVICE_AUDIO = "/dev/dsp";

/* STANDARD FREQUENCIES (Hz) OF MUSICAL NOTES FOR INSTRUMENTS EXCEPT PIANO
 *  OCT
 * +---+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
 * | # |    C   |    C#  |   D    |    D#  |    E   |   F    |   F#   |   G    |  G#    |   A    |  A#    |   B    |
 * | 1 | 32.703 | 34.648 | 36.708 | 38.891 | 41.203 | 43.654 | 46.249 | 48.999 | 51.913 | 55.000 | 58.270 | 61.735 | 
 * | 2 | 65.406 | 69.296 | 73.416 | 77.782 | 82.407 | 87.307 | 92.499 | 97.999 | 103.83 | 110.00 | 116.54 | 123.47 | 
 * | 3 | 130.81 | 138.59 | 146.83 | 155.56 | 164.81 | 174.61 | 185.00 | 196.00 | 207.65 | 220.00 | 233.08 | 246.94 | 
 * | 4 | 261.63 | 277.18 | 293.66 | 311.13 | 329.63 | 349.23 | 369.99 | 392.00 | 415.30 | 440.00 | 466.16 | 493.88 | 
 * | 5 | 523.25 | 554.37 | 587.33 | 622.25 | 659.26 | 698.46 | 739.99 | 783.99 | 830.61 | 880.00 | 932.33 | 987.77 | 
 * | 6 | 1046.5 | 1108.7 | 1174.7 | 1244.5 | 1318.5 | 1396.9 | 1480.0 | 1568.0 | 1661.2 | 1760.0 | 1864.7 | 1975.5 | 
 * | 7 | 2093.0 | 2217.5 | 2349.3 | 2489.0 | 2637.0 | 2793.8 | 2960.0 | 3136.0 | 3322.4 | 3520.0 | 3729.3 | 3951.1 | 
 * | 8 | 4186.0 | 4434.9 | 4698.6 | 4978.0 | 5274.0 | 5587.7 | 5919.9 | 6271.9 | 6644.9 | 7040.0 | 7458.6 | 7902.1 | 
 * +---+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
 */
Note NOTES[] = {
	{"C", 64.406},
	{"C", 130.81},
	{"C", 261.63},
	{"C", 523.25},
	{"C", 1046.5},
	{"C#", 69.296},
	{"C#", 138.59},
	{"C#", 277.18},
	{"C#", 554.37},
	{"C#", 1108.7},
	{"D", 73.416},
	{"D", 146.83},
	{"D", 293.66},
	{"D", 587.33},
	{"D", 1174.7},
	{"D#", 77.782},
	{"D#", 155.56},
	{"D#", 311.13},
	{"D#", 622.25},
	{"D#", 1244.5},
	{"E", 82.407},
	{"E", 164.81},
	{"E", 329.63},
	{"E", 659.26},
	{"E", 1318.5},
	{"F", 87.307},
	{"F", 174.61},
	{"F", 349.23},
	{"F", 698.46},
	{"F", 1396.9},
	{"F#", 92.499},
	{"F#", 185.00},
	{"F#", 369.99},
	{"F#", 739.99},
	{"F#", 1480.0},
	{"G", 97.999},
	{"G", 196.00},
	{"G", 392.00},
	{"G", 783.99},
	{"G", 1568.0},
	{"G#", 103.83},
	{"G#", 207.65},
	{"G#", 415.30},
	{"G#", 830.61},
	{"G#", 1661.2},
	{"A", 55},
	{"A", 110},
	{"A", 220},
	{"A", 440},
	{"A", 880},
	{"A", 1760},
	{"A#", 58.270},
	{"A#", 116.54},
	{"A#", 233.08},
	{"A#", 466.16},
	{"A#", 932.33},
	{"A#", 1864.7},
	{"B", 61.735},
	{"B", 123.47},
	{"B", 246.94},
	{"B", 493.88},
	{"B", 987.77}
};
unsigned int NUM_NOTES = sizeof(NOTES) / sizeof(Note);

/* Get Data From Device */
static void get_data_from_device (GuitarTuner *tuner) {
	unsigned char input[NSAMPLES];
	double dinput[NSAMPLES];
	double output[NSAMPLES];	
	unsigned int i;
	fftw_plan p;
	double tmp;

	/* Read Device Data */
	if (read(tuner->fd, input, (NSAMPLES * sizeof(unsigned char))) < 0) {
		perror("read()");
		return;
	}

	/* 8 Bit Samples */
	for (i=0; i < NSAMPLES; i++) {		
		dinput[i] = ((double) (input[i] - 128.0))*(0.54 - 0.46 * tuner->COSINE[i]);
	}

	/* Execute FFTW */
	p = fftw_plan_r2r_1d(NSAMPLES, dinput, output, FFTW_R2HC, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);

	tuner->max = 0;
	tuner->max_index = 0;
	for (i=2; i < NSAMPLES/16; i++) {
		tmp = output[i]*output[i] + output[NSAMPLES - i]*output[NSAMPLES - i];
		if (tmp > tuner->max) {
			tuner->max = tmp;
			tuner->max_index = i;
		}
	}
}

#if 0
/* Controllo se per N volte le freq son state uguali */
static double get_current_frequence (GuitarTuner *tuner) {
	unsigned int max_index1;
	unsigned int max_index2;	
	double max1, max2;
	unsigned int i;

	do {
		get_data_from_device(tuner);
		max_index1 = tuner->max_index;
		max1 = tuner->max;
		for (i=0; i < 2; i++) {
			get_data_from_device(tuner);
			max_index2 = tuner->max_index;
			max2 = tuner->max;

			if (max_index1 != max_index2) break;
			max_index1 = max_index2;
			max1 = max2;
		}
	} while (i < 2);

	tuner->max = max2;
	tuner->max_index = max_index1;
	return((double) max_index1 * tuner->frequency);
}
#endif

int tuner_sound_device_init (GuitarTuner *tuner, const char *device) {
	if (device == NULL)
		device = DEFAULT_DEVICE_AUDIO;

	/* Open Sound Device */
	if ((tuner->fd = open(device, O_RDONLY)) < 0) {
		perror("open()");
		return(-1);
	}

	/* Channels Setting */
	if (ioctl(tuner->fd, SNDCTL_DSP_CHANNELS, &tuner->channels) < 0) {
		perror("Channels Setting - ioctl()");
		goto snd_dev_err_close;
	}

	/* Sound Format */
	if (ioctl(tuner->fd, SNDCTL_DSP_SETFMT, &tuner->format) < 0) {
		perror("Sound Format - ioctl()");
		goto snd_dev_err_close;
	}

	/* Sampling Rate */
	if (ioctl(tuner->fd, SNDCTL_DSP_SPEED, &tuner->sampling_frequency) < 0) {
		perror("Sampling Rate - ioctl()");
		goto snd_dev_err_close;
	}

	/* All Seems OK, Return Device Descriptor */
	return(tuner->fd);

	/* Error: Close Snd Device and Return Error */
snd_dev_err_close:
	close(tuner->fd);
	return(-1);
}

void tuner_init (GuitarTuner *tuner) {
	unsigned int i;

	tuner->fd = -1;
	tuner->channels = 0;
	tuner->format = AFMT_S8;
	tuner->sampling_frequency = 22050;
	tuner->frequency = ((double) tuner->sampling_frequency/NSAMPLES);

	for (i=0; i < NSAMPLES; i++)
		tuner->COSINE[i] = cos(2.0 * M_PI * (double) i / NSAMPLES);

	tuner->reference_freq = 0.0;
	tuner->actual_freq = 0.0;
	tuner->note = NULL;
	tuner->adjust = TUNER_ADJUST_NONE;
}

short int tuner_get_note (GuitarTuner *tuner) {	
	double delta = 0;
	unsigned int i;	
	double freq;

	get_data_from_device(tuner);
	freq = (double) tuner->max_index * tuner->frequency;

#ifdef DEBUG
	fprintf(stderr, "Freq: %.3fHz (Index: %u)\n", freq, tuner->max_index);
#endif

	/* Delta is How far off we can be from a given note 
	 * (changes with octaves) It is Sharp or Flat.
	 */
	if (freq > 31 && freq < 63) delta = 2;
	else if (freq > 63 && freq < 128) delta = 4;
	else if (freq > 128 && freq < 192) delta = 8;
	else if (freq > 192 && freq < 256) delta = 12;
	else if (freq > 256 && freq < 384) delta = 16;
	else if (freq > 384 && freq < 512) delta = 24;
	else if (freq > 512 && freq < 1024) delta = 32;
	else if (freq > 1024 && freq < 2048) delta = 64;
	else if (freq > 2048 && freq < 5096) delta = 128;
	else if (freq > 5096 && freq < 8100) delta = 256;


	if (tuner->max <= (double) 10000000000.00) {
		tuner->reference_freq = 0.0;
		tuner->actual_freq = 0.0;
		tuner->note = NULL;
		tuner->adjust = TUNER_ADJUST_NONE;
		return(0);
	}

	for (i=0; i < NUM_NOTES; i++) {
		if (fabs(NOTES[i].freq - freq) < delta/2) {
			if (fabs(NOTES[i].freq - freq) < (tuner->frequency / 2.0)) {
				tuner->reference_freq = NOTES[i].freq;
				tuner->actual_freq = freq;
				tuner->note = NOTES[i].label;
				tuner->adjust = TUNER_ADJUST_NONE;
			} else if (NOTES[i].freq < freq) {
				tuner->reference_freq = NOTES[i].freq;
				tuner->actual_freq = freq;
				tuner->note = NOTES[i].label;
				tuner->adjust = TUNER_ADJUST_DOWN;
			} else if (NOTES[i].freq > freq) {
				tuner->reference_freq = NOTES[i].freq;
				tuner->actual_freq = freq;
				tuner->note = NOTES[i].label;
				tuner->adjust = TUNER_ADJUST_UP;
			}
		}
	}
	return(1);
}

