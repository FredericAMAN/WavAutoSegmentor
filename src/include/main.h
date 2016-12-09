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

#ifndef _MAIN_H_
#define _MAIN_H_

//typedef float SAMPLE;

typedef struct {
	int frameIndex; // Indice de l'échantillon du début la frame en cours
	short int *recordedFrames; //Données de l'enregistrement (toutes les frames enregistrées)
	short int *detected_signal_with_time_memo;
	short int *detected_signal;
	int size_detected_signal_with_time_memo;
	int size_detected_signal;
} data_recording;

struct globalArgs_t {
	char* inputRep;					/* -i option */
	char* outputRep;				/* -o option */
	double thresholdStart;			/* -s option */
	double thresholdEnd;			/* -e option */
	double timeBefore;				/* -b option */
	double timeAfter;				/* -a option */
	double timePending;				/* -p option */
} globalArgs;

int num_total;
int num_processed;
int num_skipped;

#endif
