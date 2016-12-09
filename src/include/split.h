/*
     Copyright (C) 2016 Laboratoire d'Informatique de Grenoble (www.liglab.fr),
     Centre National de la Recherche Scientifique (www.cnrs.fr).
     The initial author of the original code is Frédéric Aman.
     Contributors: Michel Vacher
     
     This file is part of WavAutoSegmentor.

     WavAutoSegmentor is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     WavAutoSegmentor is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with WavAutoSegmentor. If not, see <http://www.gnu.org/licenses/>.

     WavAutoSegmentor is a software project initiated by the French Centre National 
     de la Recherche Scientifique (CNRS) and the Laboratoire d'Informatique de 
     Grenoble  (LIG) - France (https://www.liglab.fr/).
     WavAutoSegmentor is software to automatically cut WAVE files by detecting 
     audio activity in the signal.

     Contact Frédéric Aman <frederic.aman@gmail.com> for more information 
     about the licence or the use of WavAutoSegmentor.
*/

#ifndef SPLIT_H
#define	SPLIT_H

/* include ================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include "detection.h"

//#include "config.h"
//#include "sound_object.h"


/* typedef et structures ==================================================== */

typedef struct {
	short int bits_per_sample;
	short int num_channels;
	int data_size;
	int sample_rate;
} header_file;

/* variables globales ======================================================= */

/* fonctions ================================================================ */
void split_init();
int split_process(char *file);
int split_wav(int start, int end, char str[], short int bits, short int stereo, int sample_rate);
int lire_entete_wav(FILE *f, short int *bits, short int *stereo, int *nr_ech);
void ecrire_entete_wav(FILE *f,int *freq_ech,short int *bits,short int *stereo,int nr_ech);
int copy_file(char const * const source, char const * const destination);
double calcul_SNR_evt(short int* signal, int start, int end, double snr_power_ref, int samples_per_buffer);
#endif
