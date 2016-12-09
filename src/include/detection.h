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

#ifndef DETECTION_H
#define	DETECTION_H

/* include ================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "trait_signal.h"
#include "main.h"

/* constantes =============================================================== */
////#define SAMPLES_PER_BUFFER	2048
#define PA_SAMPLE_TYP		paFloat32
#define SAMPLE_SILENCE		0.0f
#define DEPTH_TREE		3
#define NB_POWER		3	// niveaux d'ondelette
#define NB_MAX_MEAN		10	// Taille tableau des moyennes
//#define THRESHOLD_START		0.0000005
//#define THRESHOLD_END		0.0000005
#define OV			1.2f
//#define TMAX_PAUSE		8
#define BUFFSIZE		2048
#define SIZE_MEMO_POWER		50	// Pour calcul puissance de référence
//#define TIME_MEMO_START		0
//#define TIME_MEMO_END		0

/* typedef et structures ==================================================== */

typedef struct {
	int studied_power[NB_POWER]; // 8,9,10 (3 plus niveaux (en fréquence) d'ondelette de la DWT sur 2048ech)
	double e_mean; // valeur moyenne de l'énergie des 10 ou 20 dernières fenêtres
	double ebuf[NB_MAX_MEAN]; // mémorisation de l'énergie des 10 dernières fenêtres
	int jener; // nombre réel de fenêtres mémorisées (<=10)
	double threshold_start; // seuil début adaptable
	double threshold_end; // seuil fin adaptable
	int tpause; // Si Tpause est plus grand qu'une valeur donnée c'est la fin de la detection
	int start_detection; // Nombre d'echantillons précédant la détection dans la fenêtre de 2048 (erreur<128 echantillons)
	int end_detection; // Nombre d'échantillons suivant la detection dans la fenêtre de 2048 (erreur<128 echantillons)
	int start_memo; // Nombre d'echantillons - TIME_MEMO_START * SAMPLE_RATE précédant la détection dans la fenêtre de 2048 (erreur<128 echantillons)
	int end_memo; // Nombre d'échantillons + TIME_MEMO_END * SAMPLE_RATE suivant la detection dans la fenêtre de 2048 (erreur<128 echantillons)
	int detection_state;
	//double energy_win;
	double energy_ant;
	int nbrechenerg;
	double power_buf[SIZE_MEMO_POWER];	// Tableau des n dernière puissances (hors détection)
	int index_snr;		// Indice pour le tableau des puissances
	double snr_power_ref;
	double snr_power_evt;
	short int *state_detection;
	short int *is_creating_wav;
	short int *tab_split_wav;
	int flag_create_wav;
	int prev_start_detection; // Nombre d'echantillons précédant la détection dans la fenêtre de 2048 (erreur<128 echantillons)
	int prev_end_detection; // Nombre d'échantillons suivant la detection dans la fenêtre de 2048 (erreur<128 echantillons)
} data_detection;

enum {
	DETECTION_IN_PROGESS,
	DETECTION_DONE,
	DETECTION_START,
	DETECTION_PAUSE
};

/* variables globales ======================================================= */


/* fonctions ================================================================ */
void init_detection(data_detection* G_Detection);
int detection(short int *buffer, int *frame_index, int samples_per_buffer, data_detection* G_Detection, int sample_rate);
#endif

